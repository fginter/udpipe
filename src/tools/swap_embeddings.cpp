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
#include "model/pipeline.h"
#include "sentence/input_format.h"
#include "sentence/output_format.h"
#include "tokenizer/detokenizer.h"
#include "trainer/trainer.h"
#include "utils/getpara.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "utils/process_args.h"
#include "version/version.h"
#include "utils/parse_int.h"
#include "utils/compressor.h"

using namespace ufal::udpipe;


int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"precision", options::value::any},
	  {"version", options::value::none},
	    {"help", options::value::none}}, argc, argv, options) ||
    options.count("help") ||
    (argc < 4 && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << " [options] [extract|replace] embedding_name model_file\n"
		    "Options: --precision=precision of extraced embeddings [6]\n"
		    "         --version\n"
		    "         --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

  if (argv[1] != string("extract") && argv[1] != string("replace"))
    runtime_failure("The first argument must be either 'extract' or 'replace'!");
  bool extract = argv[1] == string("extract");
  int precision = options.count("precision") ? parse_int(options["precision"], "precision option") : 6;
  string requested_embedding = argv[2];
  string model_file = argv[3];
  
  // Load model
  unique_ptr<model> model;
  cerr << "Loading UDPipe model: " << flush;
  model.reset(model::load(model_file.c_str()));
  if (!model) runtime_failure("Cannot load UDPipe model '" << model_file << "'!");
  cerr << "done." << endl;

  
  
  return 0;
}
