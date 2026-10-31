// Original entrant implementation, begun after 2026-10-30 15:00 UTC kickoff.
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

struct Group {
  std::vector<uint32_t> latency;
  uint64_t errors = 0;
};
struct Endpoint {
  std::string name;
  std::array<Group, 72> hours;
  explicit Endpoint(std::string_view s) : name(s) {}
};
struct Hash {
  using is_transparent = void;
  size_t operator()(std::string_view s) const {
    return std::hash<std::string_view>{}(s);
  }
};

int main(int argc, char **argv) {
  const std::string input = std::string(argc > 1 ? argv[1] : "/data") + "/requests.log";
  const std::string output = std::string(argc > 2 ? argv[2] : "/out") + "/summary.txt";
  FILE *in = fopen(input.c_str(), "rb");
  if (!in) { perror("requests.log"); return 1; }
  setvbuf(in, nullptr, _IOFBF, 1 << 20);
  std::unordered_map<std::string, std::unique_ptr<Endpoint>, Hash, std::equal_to<>> endpoints;
  endpoints.reserve(2048);
  std::array<std::string, 72> hours;
  char *line = nullptr;
  size_t capacity = 0;
  while (getline(&line, &capacity, in) != -1) {
    const unsigned month = (line[5] - '0') * 10 + line[6] - '0';
    const unsigned day = (line[8] - '0') * 10 + line[9] - '0';
    const unsigned hour = (line[11] - '0') * 10 + line[12] - '0';
    // The published grammar fixes the span to Oct 30 18:00 through Nov 2 17:59.
    const unsigned bucket = (month == 10 ? day - 30 : day + 1) * 24 + hour - 18;
    if (bucket >= hours.size()) { free(line); fclose(in); return 2; }
    if (hours[bucket].empty()) hours[bucket] = std::string(line, 13) + ":00:00Z";
    const char *p = line + 28; // Skip the fixed-width timestamp and its space.
    while (*p != ' ') ++p; // Method does not affect aggregation.
    const char *start = ++p;
    while (*p != ' ') ++p;
    std::string_view name(start, p - start);
    auto found = endpoints.find(name);
    if (found == endpoints.end()) {
      auto endpoint = std::make_unique<Endpoint>(name);
      found = endpoints.emplace(endpoint->name, std::move(endpoint)).first;
    }
    auto &group = found->second->hours[bucket];
    ++p;
    group.errors += *p == '5'; // Status grammar is exactly three digits, 100..599.
    p += 4;
    uint32_t latency = 0;
    while (*p != '\n') { latency = latency * 10 + (*p++ - '0'); }
    group.latency.push_back(latency);
  }
  bool failed = ferror(in);
  free(line);
  fclose(in);
  if (failed) return 3;
  std::vector<Endpoint *> ordered;
  for (auto &[name, endpoint] : endpoints) ordered.push_back(endpoint.get());
  std::sort(ordered.begin(), ordered.end(), [](auto a, auto b) { return a->name < b->name; });
  FILE *out = fopen(output.c_str(), "wb");
  if (!out) { perror("summary.txt"); return 4; }
  setvbuf(out, nullptr, _IOFBF, 1 << 20);
  for (size_t hour = 0; hour < hours.size(); ++hour) {
    for (auto endpoint : ordered) {
      auto &group = endpoint->hours[hour];
      auto &values = group.latency;
      const uint64_t n = values.size();
      if (!n) continue;
      std::sort(values.begin(), values.end());
      auto percentile = [&](uint64_t p) { return values[(p * n + 99) / 100 - 1]; };
      fprintf(out, "%s %s %llu %u %u %u %llu %llu\n", hours[hour].c_str(),
              endpoint->name.c_str(), static_cast<unsigned long long>(n),
              percentile(50), percentile(95), percentile(99),
              static_cast<unsigned long long>(group.errors),
              static_cast<unsigned long long>(group.errors * 1000000 / n));
    }
  }
  failed = ferror(out);
  if (fclose(out) != 0) failed = true;
  return failed ? 5 : 0;
}
