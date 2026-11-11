//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//
#include "stdafx.hpp"

namespace nwk {
	constexpr auto k_r_grabaRecordSize = 400U;

	struct c_var {
		bool activo;   // bool is 1 byte, but 3 bytes padding to let padre begin at 4 byte boundary?
		int32_t padre;
		int32_t nvar;  // number of the variable, negative if static
		char nombre[14];
		char dummy1[2];
		int32_t link;  // type of link
		char  desc[40];
		int32_t dummy2;
		int32_t valori;
		int32_t valori_b;  //  valori_b:boolean ...boolean is 32 bit in Pascal?
		char varl[20];
		int32_t longitud;
		int32_t device;
		int32_t entrada;   // input or output
		char tipo2[2];
		char dummy3[2];
		int32_t digito1;  // digit
		int32_t cifras;   // numbers
	};

	struct r_graba {  // 400 bytes in total; MUST BE EQUAL TO kRecord_size  !
		uint8_t vers;   // version
		int32_t contador;
		char des[120];
		uint8_t tipo;  // 0=variable, 1=line of commands
		c_var v;
		char dummy4[140];    // make it fit 400 bytes
	};


	// source Ferdie from Opencockpits, oktober 2008:

	//  The .ssi structure is binary file with this register (is a tree) :

	// r_graba = record
	//  vers:byte; // version
	//  contador:integer;
	//  des:std::string[120]; // description
	//  tipo:byte; // 0=variable, 1=line of commands
	//  v:c_var; // register for variable
	//  c:c_lin; // register for line of commands
	// end;
	//
	// c_var = record
	//  activo:boolean;
	//  padre:integer;
	//  nvar:integer; // number of the variable, negative if static
	//  nombre:std::string[14];
	//  link:integer; // type of link
	//  desc:std::string[40];
	//  valori:integer;
	//  valori_b:boolean;
	//  varl:std::string[20];
	//  longitud:integer;
	//  device:integer;
	//  entrada:integer;
	//  tipo:std::string[2];
	//  digito1:integer;
	//  cifras:integer;
	//  aceleracion:integer;
	//  posi,posc,posd:integer;
	// end;
	//
	// c_lin = record
	//  activo:boolean;
	//  padre:integer;
	//  tipo:integer;  // type of cmnd
	//  comments:std::string[40];
	//  funcion:integer;
	//  valor1:real;
	//  valor2:real;
	//  valor3:real;
	//  valor4:real;
	//  vari1:integer;
	//  vari2:integer;
	//  vari3:integer;
	//  vari4:integer;
	//  operador:integer;
	// end;

	SsiFileReader::SsiFileReader(const Qcode static_var_from, const Qcode static_var_to)  // static_var_to inclusive
		:
		static_var_from_{ static_var_from },
		static_var_to_{ static_var_to } {
#ifndef PMDG_NGX
		static_assert(k_r_grabaRecordSize == static_cast<uint32_t>(sizeof(r_graba)));
#endif
	}

	SsiFileReader::~SsiFileReader() {
		closeFile();
	}

	auto SsiFileReader::openFile(std::filesystem::path const& ssi_file) noexcept -> bool {
		auto b{ false };
		try {
			file_.open(ssi_file.string().c_str(), std::ios_base::in | std::ios_base::binary);
			b = file_.is_open();
			if (!b) {
				file_.clear();
			}
		} catch (...) {
			gf_logUnexpectedAndUnhandledException(__func__);
		}
		return b;
	}

	auto SsiFileReader::closeFile() noexcept -> void {
		try {
			if (file_.is_open()) {
				file_.close();
			}
		} catch (...) {
			gf_logUnexpectedAndUnhandledException(__func__);
		}
	};

	auto SsiFileReader::collectStaticVarNumbers() -> void {
		// get length of file:
		file_.seekg(static_cast<std::streamoff>(0), std::ios::end);
		const auto length{ static_cast<uint32_t>(file_.tellg()) };
		int32_t previousvar{99'999};
		for (auto icur{ 0U }; icur < length; icur += k_r_grabaRecordSize) {
			static_cast<void>(file_.seekg(static_cast<std::streamoff>(icur), std::ios::beg));
			r_graba record;
			static_cast<void>(file_.read(reinterpret_cast<char*>(&record), static_cast<std::streamsize>(k_r_grabaRecordSize)));
			if (const auto var = record.v.nvar; var < 0 && var != previousvar) {
				previousvar = var;
				const auto usvar{ static_cast<uint32_t>(-var) }; // static var numbers are negative!
				if (usvar >= static_cast<uint32_t>(static_var_from_) && usvar <= static_cast<uint32_t>(static_var_to_)) {
					static_vars_.insert(static_cast<Qcode>(usvar));
				}
			}
		}
	}
}  
