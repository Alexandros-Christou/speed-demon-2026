// Original Track C implementation, begun 30 October 2026 after kickoff.
#include <array>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class Csv {
  FILE *file;
  std::array<unsigned char, 1 << 20> buffer;
  size_t position = 0, available = 0;
  int current;
  int next() {
    if (position == available) {
      available = fread(buffer.data(), 1, buffer.size(), file);
      position = 0;
      if (!available) {
        if (ferror(file)) throw std::runtime_error("input read failed");
        return EOF;
      }
    }
    return buffer[position++];
  }
  void field(std::string &value) {
    value.clear();
    if (current == '"') {
      current = next();
      for (;;) {
        if (current == EOF) throw std::runtime_error("unterminated quoted field");
        if (current == '"') {
          current = next();
          if (current != '"') break;
        }
        value.push_back(static_cast<char>(current));
        current = next();
      }
    } else {
      while (current != EOF && current != ',' && current != '\r' && current != '\n') {
        if (current == '"') throw std::runtime_error("quote in unquoted field");
        value.push_back(static_cast<char>(current));
        current = next();
      }
    }
  }
public:
  explicit Csv(const std::string &path) : file(fopen(path.c_str(), "rb")) {
    if (!file) throw std::runtime_error("cannot open input");
    current = next();
  }
  ~Csv() { fclose(file); }
  bool record(std::vector<std::string> &row) {
    if (current == EOF) return false;
    if (current == 0xef) {
      if (next() != 0xbb || next() != 0xbf) throw std::runtime_error("invalid BOM");
      current = next();
    }
    for (size_t i = 0; i < row.size(); ++i) {
      field(row[i]);
      if (i + 1 < row.size()) {
        if (current != ',') throw std::runtime_error("missing CSV column");
        current = next();
      }
    }
    if (current == '\r') {
      current = next();
      if (current != '\n') throw std::runtime_error("invalid record terminator");
    }
    if (current == '\n') current = next();
    else if (current != EOF) throw std::runtime_error("extra CSV column");
    return true;
  }
};

uint64_t number(const std::string &s) {
  if (s.empty()) throw std::runtime_error("empty number");
  uint64_t value = 0;
  for (unsigned char c : s) {
    if (c < '0' || c > '9') throw std::runtime_error("invalid number");
    value = value * 10 + c - '0';
  }
  return value;
}
int64_t cents(const std::string &s) {
  bool negative = !s.empty() && s[0] == '-';
  size_t start = negative ? 1 : 0;
  if (s.size() < start + 4 || s[s.size() - 3] != '.') throw std::runtime_error("invalid amount");
  int64_t value = number(s.substr(start, s.size() - start - 3)) * 100 + number(s.substr(s.size() - 2));
  return negative ? -value : value;
}
std::string money(int64_t value) {
  uint64_t magnitude = value < 0 ? -value : value;
  return std::string(value < 0 ? "-" : "") + std::to_string(magnitude / 100) + "." +
         (magnitude % 100 < 10 ? "0" : "") + std::to_string(magnitude % 100);
}
struct Group {
  uint64_t count = 0, ids = 0, name_bytes = 0, note_bytes = 0;
  int64_t sum = 0, minimum = 0, maximum = 0;
  uint64_t distinct = 0;
  std::vector<uint64_t> names;
};

int main(int argc, char **argv) try {
  std::string data = argc > 1 ? argv[1] : "/data";
  std::string output = argc > 2 ? argv[2] : "/out";
  constexpr uint16_t unmatched = 26 * 26 * 26;
  std::vector<Group> groups(unmatched + 1);
  std::unordered_map<std::string, uint32_t> name_ids;
  name_ids.reserve(131072);
  std::vector<uint16_t> lookup;
  std::vector<std::string> region(2), row(5);
  Csv regions(data + "/regions.csv");
  if (!regions.record(region) || region != std::vector<std::string>{"region_id", "country"})
    throw std::runtime_error("unexpected regions header");
  while (regions.record(region)) {
    auto id = number(region[0]);
    const auto &code = region[1];
    if (code.size() != 3 || code.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos)
      throw std::runtime_error("invalid country code");
    if (id >= lookup.size()) lookup.resize(id + 1, unmatched);
    lookup[id] = (code[0] - 'A') * 676 + (code[1] - 'A') * 26 + code[2] - 'A';
  }
  Csv records(data + "/records.csv");
  if (!records.record(row) || row != std::vector<std::string>{"id", "name", "note", "region_id", "amount"})
    throw std::runtime_error("unexpected records header");
  while (records.record(row)) {
    auto id = number(row[0]), region_id = number(row[3]);
    auto amount = cents(row[4]);
    auto &g = groups[region_id < lookup.size() ? lookup[region_id] : unmatched];
    if (!g.count) g.minimum = g.maximum = amount;
    ++g.count; g.ids += id; g.sum += amount;
    if (amount < g.minimum) g.minimum = amount;
    if (amount > g.maximum) g.maximum = amount;
    // Intern exact unescaped bytes once, then record per-country membership.
    auto name = name_ids.try_emplace(row[1], static_cast<uint32_t>(name_ids.size())).first;
    size_t word = name->second / 64;
    uint64_t bit = uint64_t{1} << (name->second % 64);
    if (g.names.size() <= word) g.names.resize(word + 1, 0);
    if (!(g.names[word] & bit)) { g.names[word] |= bit; ++g.distinct; }
    g.name_bytes += row[1].size(); g.note_bytes += row[2].size();
  }
  FILE *out = fopen((output + "/aggregates.txt").c_str(), "wb");
  if (!out) throw std::runtime_error("cannot open output");
  bool failed = false;
  for (unsigned i = 0; i <= unmatched; ++i) {
    auto &g = groups[i];
    if (!g.count) continue;
    std::string key = "_UNMATCHED";
    if (i != unmatched) key = {char('A' + i / 676), char('A' + i / 26 % 26), char('A' + i % 26)};
    failed |= fprintf(out, "%s %llu %llu %s %s %s %llu %llu %llu\n", key.c_str(),
      static_cast<unsigned long long>(g.count), static_cast<unsigned long long>(g.ids),
      money(g.sum).c_str(), money(g.minimum).c_str(), money(g.maximum).c_str(), static_cast<unsigned long long>(g.distinct),
      static_cast<unsigned long long>(g.name_bytes), static_cast<unsigned long long>(g.note_bytes)) < 0;
  }
  failed |= fclose(out) != 0;
  return failed ? 1 : 0;
} catch (const std::exception &e) {
  fprintf(stderr, "%s\n", e.what()); return 1;
}
