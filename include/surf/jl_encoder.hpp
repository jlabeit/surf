#include <vector>
#include <iostream>
#include <tuple>
#include <map>

using std::vector;
using std::make_tuple;
using std::map;

typedef std::tuple<uint64_t, uint64_t> KeyType;
class jl_encoder {
  private:
    vector<KeyType> m_keys;
    map<KeyType, vector<uint64_t>> m_deltas;;

    KeyType get_key(const vector<uint64_t>& run) {
	uint64_t max_delta = run[0];				
	for (size_t i = 1; i < run.size(); ++i) {
		max_delta = std::max(max_delta, run[i] - run[i-1]);
	}
	return make_tuple(max_delta, run.size());
    }

    void add_deltas(const vector<uint64_t>& run, const KeyType& key) {
	// Check if delta list already exists.
	if (m_deltas.count(key) == 0)
		m_deltas[key] = vector<uint64_t>();
	vector<uint64_t>& d = m_deltas[key];
	d.push_back(run[0]);
	for (size_t i = 1; i < run.size(); ++i)
		d.push_back(run[i] - run[i-1]);
    }

    uint64_t rank(const KeyType& key, uint64_t pos) const {
	uint64_t res = 0;
	for (size_t i = 0; i < pos; ++i)
		if (m_keys[i] == key)
		  res++;	
	return res;
    }

  public:
    jl_encoder() {


    }

    void add(const vector<uint64_t>& run) {
	KeyType key = get_key(run);	
	m_keys.push_back(key);
	add_deltas(run, key);
    }

    vector<uint64_t> decode_run(uint64_t run_id) {
	KeyType key = m_keys[run_id];
	vector<uint64_t> result;
	uint64_t run_size = std::get<1>(key);
	result.resize(run_size);
	uint64_t offset = run_size * rank(key, run_id);
	for (size_t i = 0; i < run_size; ++i)
		result[i] = m_deltas[key][offset + i];
	// Prefix sum.
	for (size_t i = 1; i < run_size; ++i)
		result[i] += result[i-1];
	return result;
    }
};
