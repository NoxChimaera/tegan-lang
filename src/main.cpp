#include <iostream>
#include <cstdio>
#include <string>

#include "Parser/Printer.cpp"
#include "Parser/Codegen.cpp"
#include "Parser/Parser.hpp"

void addOpt( char* arg, Codegen cg ) {
  if ( strcmp( "-O1", arg ) == 0 ) {
    std::cout << "Optimization: instruction combining" << std::endl;
    cg.opt_instcombine();
  } else if ( strcmp( "-O2", arg ) == 0 ) {
    std::cout << "Optimization: reassociate" << std::endl;
    cg.opt_reassociate();
  } else if ( strcmp( "-O3", arg ) == 0 ) {
    std::cout << "Optimization: dead code elimination" << std::endl;
    cg.opt_dce();
  } else if ( strcmp( "-O4", arg ) == 0 ) {
    std::cout << "Optimization: global value numbering" << std::endl;
    cg.opt_gvn();
  } else if ( strcmp( "-O5", arg ) == 0 ) {
    std::cout << "Optimization: simplify CFG" << std::endl;
    cg.opt_simplifyCFG();
  } else if ( strcmp( "-Oall", arg ) == 0 ) {
    std::cout << "Optimization: instruction combining, " << std::endl
      << "\t reassociate, " << std::endl
      << "\t dead code elimination, " << std::endl
      << "\t global value numbering, " << std::endl
      << "\t simplify CFG" << std::endl;
    cg.opt_instcombine();
    cg.opt_reassociate();
    cg.opt_dce();
    cg.opt_gvn();
    cg.opt_simplifyCFG();
  }
}

int main( int argc, char* argv[] ) {
  std::cout << "Tegan Language Compiler v.0.1"
    << std::endl<< std::endl << std::endl;

  FILE* file = fopen( argv[ 1 ], "r" );
  if ( file == 0 ) {
    std::cout << "Can't open file " << argv[ 1 ] << "\n";
    return 1;
  }

  if ( argv[ 2 ] == NULL ) {
    std::cout << "Specify output file" << std::endl;
    return 1;
  }
  char* output = argv[ 2 ];

  auto parser = Parser();
  Node* root = parser.parse( file );
  std::cout << std::endl;

  Printer printer = Printer();
  Codegen codegen = Codegen();

  for ( int i = 3; i < argc; ++i ) {
    addOpt( argv[ i ], codegen );
  }

  if ( root != NULL ) {
    std::cout << std::endl;
    root->accept( printer );

    if ( !parser.isFailed() ) {
      std::cout << std::endl;
      root->accept( codegen );

      codegen.dump();
      codegen.save( output );
    }
  } else {
    std::cout << "EXIT" << std::endl;
  }
  std::cout << std::endl << std::endl << std::endl;
}
