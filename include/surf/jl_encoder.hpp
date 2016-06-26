#include <vector>
#include <iostream>
#include <tuple>
#include <map>
#include <sdsl/bit_vectors.hpp>

using std::vector;
using std::make_tuple;
using std::map;
using sdsl::bit_vector;

typedef std::tuple<uint64_t, uint64_t> KeyType;
const KeyType BIG_RUN = make_tuple(-1,-1);
class jl_encoder{
  private:
    // Number of runs already encoded.
    uint64_t m_cur_run; 
    // List that stores for all short runs their key. 
    vector<KeyType> m_keys;
    // For each key a list of all deltas of runs with the corresponding key.
    map<KeyType, vector<uint64_t>> m_deltas;
    map<KeyType, int_vector<>> m_compressed_deltas;
    vector<int_vector<>> m_big_runs;
    // Get key of a run.
    KeyType get_key(const vector<uint64_t>& run) {
	if (run.size() > 15)
		return BIG_RUN;
	uint64_t max_delta = run[0];				
	for (size_t i = 1; i < run.size(); ++i) {
		max_delta = std::max(max_delta, run[i] - run[i-1]);
	}
	if (max_delta > 15)
		return BIG_RUN;
	return make_tuple(max_delta, run.size());
    }
    // Encode all deltas of a run.
    void add_deltas(const vector<uint64_t>& run, const KeyType& key) {
	// For big runs just sore runs explicitly.
	if (key == BIG_RUN) {
		int_vector<> deltas(run.size());
		deltas[0] = run[0];
		for (size_t i = 1; i < run.size(); ++i)
			deltas[i] = run[i] - run[i-1];
		util::bit_compress(deltas);
		m_big_runs.push_back(deltas);
	} else {
		// Check if delta list already exists.
		if (m_deltas.count(key) == 0)
			m_deltas[key] = vector<uint64_t>();
		vector<uint64_t>& d = m_deltas[key];
		d.push_back(run[0]);
		for (size_t i = 1; i < run.size(); ++i)
			d.push_back(run[i] - run[i-1]);
	}
    }
    // Count how many runs with a key have already been encoded before position pos.
    uint64_t rank(const KeyType& key, uint64_t pos) const {
	uint64_t res = 0;
	for (size_t i = 0; i < pos; ++i)
		if (m_keys[i] == key)
		  res++;	
	return res;
    }

  public:
    jl_encoder(): m_cur_run(0) {}

    void add(const vector<uint64_t>& run) {
	KeyType key = get_key(run);	
	m_keys.push_back(key);
	add_deltas(run, key);
	m_cur_run++;
    }

    void prepare_decode() {
	for(const auto& kv : m_deltas) {
		int_vector<> c_deltas(kv.second.size(), 0);
		for (size_t i = 0; i < kv.second.size(); ++i)
			c_deltas[i] = kv.second[i];
		util::bit_compress(c_deltas);
		m_compressed_deltas[kv.first] = c_deltas;
	}
	m_deltas.clear();
    }

    int_vector<64> decode_run(uint64_t run_id) {
	KeyType key = m_keys[run_id];
	int_vector<64> result;
	if (key == BIG_RUN) {
		const int_vector<>& tmp = m_big_runs[rank(key, run_id)];
		result.resize(tmp.size());
		for (size_t i = 0; i < tmp.size(); ++i)
			result[i] = tmp[i];
	} else {
		uint64_t run_size = std::get<1>(key);
		result.resize(run_size);
		uint64_t offset = run_size * rank(key, run_id);
		for (size_t i = 0; i < run_size; ++i)
			result[i] = m_compressed_deltas[key][offset + i];
	}
	// Prefix sum.
	for (size_t i = 1; i < result.size(); ++i)
		result[i] += result[i-1];
	return result;
    }
};
