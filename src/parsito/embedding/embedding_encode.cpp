// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "common.h"
#include "embedding.h"
#include <fstream>
#include "utils/split.h"
#include "utils/parse_double.h"
#include "utils/parse_int.h"


namespace ufal {
namespace udpipe {
namespace parsito {

void embedding::save(binary_encoder& enc) const {
  // Save dimension and update_weight
  enc.add_4B(dimension);

  // Save the dictionary
  vector<string_piece> words(dictionary.size());
  for (auto&& entry : dictionary) {
    assert(entry.second >= 0 && entry.second < int(dictionary.size()));
    words[entry.second] = entry.first;
  }
  enc.add_4B(dictionary.size());
  for (auto&& word : words)
    enc.add_str(word);

  enc.add_1B(unknown_index >= 0);

  // Save the weights
  enc.add_data(weights);
}

bool embedding::can_update_weights(int id) const {
  return id >= int(updatable_index);
}

  void embedding::read_from_txtfile(string fname) {

    ifstream in(fname.c_str());
    if (!in.is_open()) cerr << "Cannot open " << fname << endl;

    // Load first line containing dictionary size and dimensions
    string line;
    vector<string_piece> parts;
    if (!getline(in, line)) cerr << "Cannot read first line from embedding file '" << fname << "'!";
    split(line, ' ', parts);
    if (parts.size() != 2) cerr << "Expected two numbers on the first line of embedding file '" << fname << "'!";
    int file_dimension = parse_int(parts[1], "embedding file dimension");
    int dict_size=parse_int(parts[0], "embedding file dictsize");
    
    // Load input embedding
    vector<double> lweights(file_dimension);
    int counter=0;
    while (getline(in, line)) { //&& int(weights.size()) < max_embeddings) {
        split(line, ' ', parts);
        if (!parts.empty() && !parts.back().len) parts.pop_back(); // Ignore space at the end of line
        if (int(parts.size()) != file_dimension + 1) cerr << "Wrong number of values on line '" << line << "' of embedding file '" << fname << endl;
        for (int i = 0; i < file_dimension; i++)
          lweights[i] = parse_double(parts[1 + i], "embedding weight");

        string word(parts[0].str, parts[0].len);

	dictionary.emplace(word,counter);
	weights.insert(weights.end(), lweights.begin(), lweights.end());
	counter++;
      }
    //      embeddings_from_file = weights.size();
    //  updatable_index = update_weights ? 0 : embeddings_from_file;
}
  
void embedding::create(unsigned dimension, int updatable_index, const vector<pair<string, vector<float>>>& words, const vector<float>& unknown_weights) {
  this->dimension = dimension;
  this->updatable_index = updatable_index;

  dictionary.clear();
  weights.clear();
  for (auto&& word : words) {
    assert(word.second.size() == dimension);
    dictionary.emplace(word.first, dictionary.size());
    weights.insert(weights.end(), word.second.begin(), word.second.end());
  }

  if (unknown_weights.empty()) {
    this->unknown_index = -1;
  } else {
    this->unknown_index = dictionary.size();
    weights.insert(weights.end(), unknown_weights.begin(), unknown_weights.end());
  }
}

void embedding::export_embeddings(vector<pair<string, vector<float>>>& words, vector<float>& unknown_weights) const {
  words.clear();
  unknown_weights.clear();

  if (dictionary.empty()) return;

  assert(unknown_index < 0 || unknown_index == int(dictionary.size()));

  words.resize(dictionary.size());
  for (auto&& entry : dictionary) {
    words[entry.second].first = entry.first;
    words[entry.second].second.assign(weights.data() + entry.second * dimension, weights.data() + entry.second * dimension + dimension);
  }
  if (unknown_index >= 0)
    unknown_weights.assign(weights.data() + unknown_index * dimension, weights.data() + unknown_index * dimension + dimension);
}

} // namespace parsito
} // namespace udpipe
} // namespace ufal
