// Original Track D implementation, begun after the 30 October 2026 kickoff.
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class Input {
  FILE *file;
  std::array<unsigned char, 1 << 20> buffer;
  size_t at = 0, length = 0;
  int byte() {
    if (at == length) {
      length = fread(buffer.data(), 1, buffer.size(), file); at = 0;
      if (!length) {
        if (ferror(file)) throw std::runtime_error("read failed");
        return EOF;
      }
    }
    return buffer[at++];
  }
public:
  explicit Input(const std::string &path) : file(fopen(path.c_str(), "rb")) {
    if (!file) throw std::runtime_error("cannot open input");
  }
  ~Input() { fclose(file); }
  bool number(uint32_t &value) {
    int c = byte();
    if (c == EOF) return false;
    if (c < '0' || c > '9') throw std::runtime_error("invalid number");
    uint64_t result = 0;
    do {
      result = result * 10 + c - '0';
      if (result > UINT32_MAX) throw std::runtime_error("integer overflow");
      c = byte();
    } while (c >= '0' && c <= '9');
    if (c != ' ' && c != '\n' && c != EOF) throw std::runtime_error("invalid separator");
    value = result; return true;
  }
  uint32_t required() {
    uint32_t value;
    if (!number(value)) throw std::runtime_error("missing field");
    return value;
  }
};

struct Edge { uint32_t target, weight; };
struct Raw { uint32_t source, target, weight; };
using Item = std::pair<uint64_t, uint32_t>;
constexpr uint64_t infinity = std::numeric_limits<uint64_t>::max();

int main(int argc, char **argv) try {
  std::string data = argc > 1 ? argv[1] : "/data";
  std::string output = argc > 2 ? argv[2] : "/out";
  Input graph(data + "/graph.txt");
  uint32_t n = graph.required(), m = graph.required();
  if (!n) throw std::runtime_error("empty node set");
  std::vector<uint32_t> offsets(n + 1), indegree(n), parents(n);
  for (uint32_t i = 0; i < n; ++i) parents[i] = i;
  auto root = [&](uint32_t u) {
    while (parents[u] != u) { parents[u] = parents[parents[u]]; u = parents[u]; }
    return u;
  };
  std::vector<Raw> raw;
  raw.reserve(m);
  for (uint32_t i = 0; i < m; ++i) {
    uint32_t u = graph.required(), v = graph.required(), w = graph.required();
    if (u >= n || v >= n) throw std::runtime_error("invalid edge node");
    raw.push_back({u, v, w}); ++offsets[u + 1]; ++indegree[v];
    auto a = root(u), b = root(v);
    if (a != b) parents[a] = b;
  }
  for (uint32_t i = 0; i < n; ++i) offsets[i + 1] += offsets[i];
  std::vector<Edge> edges(m);
  {
    auto cursor = offsets;
    for (auto e : raw) edges[cursor[e.source]++] = {e.target, e.weight};
  }
  std::vector<Raw>().swap(raw);
  for (uint32_t i = 0; i < n; ++i) parents[i] = root(i);
  std::vector<uint64_t> distance(n, infinity);
  std::vector<uint32_t> touched;
  std::vector<Item> heap;
  touched.reserve(131072); heap.reserve(131072);
  auto compare = std::greater<Item>{};
  auto push = [&](uint64_t d, uint32_t u) {
    heap.emplace_back(d, u); std::push_heap(heap.begin(), heap.end(), compare);
  };
  auto solve = [&](uint32_t source, uint32_t target) -> int64_t {
    if (source == target) return 0;
    if (parents[source] != parents[target] || !indegree[target] || offsets[source] == offsets[source + 1]) return -1;
    for (auto u : touched) distance[u] = infinity;
    touched.clear(); heap.clear();
    distance[source] = 0; touched.push_back(source); push(0, source);
    while (!heap.empty()) {
      std::pop_heap(heap.begin(), heap.end(), compare);
      auto [d, u] = heap.back(); heap.pop_back();
      if (d != distance[u]) continue;
      if (u == target) return static_cast<int64_t>(d);
      for (uint32_t i = offsets[u]; i < offsets[u + 1]; ++i) {
        auto e = edges[i]; uint64_t candidate = d + e.weight;
        if (candidate < distance[e.target]) {
          if (distance[e.target] == infinity) touched.push_back(e.target);
          distance[e.target] = candidate; push(candidate, e.target);
        }
      }
    }
    return -1;
  };
  Input queries(data + "/queries.txt");
  FILE *out = fopen((output + "/distances.txt").c_str(), "wb");
  if (!out) throw std::runtime_error("cannot open output");
  uint32_t source;
  bool failed = false;
  while (queries.number(source)) {
    auto target = queries.required();
    if (source >= n || target >= n) throw std::runtime_error("invalid query node");
    failed |= fprintf(out, "%lld\n", static_cast<long long>(solve(source, target))) < 0;
  }
  failed |= fclose(out) != 0;
  return failed ? 1 : 0;
} catch (const std::exception &e) {
  fprintf(stderr, "%s\n", e.what()); return 1;
}
