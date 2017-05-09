// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>
#include <sstream>

#include "common.h"
#include "model/evaluator.h"
#include "model/model.h"
#include "model/model_morphodita_parsito.h"
#include "model/pipeline.h"
#include "sentence/input_format.h"
#include "sentence/output_format.h"
#include "tokenizer/detokenizer.h"
#include "trainer/trainer.h"
#include "parsito/parser/parser_nn.h"
#include "utils/getpara.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "utils/process_args.h"
#include "version/version.h"
#include "utils/parse_int.h"
#include "utils/compressor.h"
#include "parsito/embedding/embedding.h"

using namespace ufal::udpipe;

int save_bin(parsito::embedding &e, string fname) {
  vector<pair<string, vector<float>>> words;
  vector<float> unknown_weights;
  ofstream fout(fname);
  cerr << "Writing to " << fname << " float size= " << sizeof(float) << endl;
  e.export_embeddings(words,unknown_weights);
  fout << words.size()+1 << " " << words[0].second.size() << "\n"; //+1 because of the unknown word "parsitounk"
  for (auto&&pair : words) {
    fout << pair.first << " ";
    for (auto&&w : pair.second) {
      fout.write((char*)(void*)&w,sizeof(float));
    }
    fout << "\n";
  }
  fout.flush();
  fout.close();
}

int save_txt(parsito::embedding &e, string fname) {
  vector<pair<string, vector<float>>> words;
  vector<float> unknown_weights;
  ofstream fout(fname);
  cerr << "Writing to " << fname << " float size= " << sizeof(float) << endl;
  e.export_embeddings(words,unknown_weights);
  fout << words.size() << " " << words[0].second.size() << "\n";
  for (auto&&pair : words) {
    fout << pair.first;
    for (auto&&w : pair.second) {
      fout << " " << w;
    }
    fout << "\n";
  }
  fout << "parsitounk";
  for (auto&&w : unknown_weights) {
    fout << " " << w;
  }
  fout << "\n";
  fout.flush();
  fout.close();
}


int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"precision", options::value::any},
  	  {"version", options::value::none},
  	    {"help", options::value::none}}, argc, argv, options) ||
    options.count("help") ||
    (argc != 2 && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << " model_file\n"
  		    "         --version\n"
  		    "         --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

  string model_file = argv[1];
  
  // Load model
  unique_ptr<model> model;
  cerr << "Loading UDPipe model: " << flush;
  model.reset(model::load(model_file.c_str()));
  if (!model) runtime_failure("Cannot load UDPipe model '" << model_file << "'!");
  cerr << "done." << endl;

  //uhhuh
  parsito::parser_nn *parser=(parsito::parser_nn*)(((model_morphodita_parsito*)model.get())->parser.get());

  vector<string> selectors={"form","lemma","lemma_id","tag","utag","feats","utagfeats","deprel"};
  for (int i=0 ; i<parser->values.size() ; i++) {
    cerr << "values " << parser->values[i].selector << endl; //->parser.embeddings_values[0[0] << endl;
    cerr << "unknown index " << parser->embeddings[i].unknown_index << endl; //->parser.embeddings_values[0[0] << endl;
    save_txt(parser->embeddings[i], model_file+string("_emb.")+selectors[parser->values[i].selector]+string(".vectors"));
  }
  // ifstream is(model_file.c_str(), ifstream::in | ifstream::binary);
  

  
  
  return 0;
}
