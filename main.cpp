#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iterator>
#include <string>
#include <regex>
#include <math.h>

using namespace std;

vector<string> split(string const &input);
bool isNumber(const string& str);
string validateNumber(string n);
string add(string n1, string n2, bool flipSignal);
string subtract(string n1, string n2, bool negative);
string multiply(string n1, string n2, bool negative);
string divide(string n1, string n2, bool negative, int cont);
string remainder(string n1, string n2, bool negative, int cont);
string power(string n1, string n2, bool negative, int cont);
void startCode(string filename);
void finishCode(string filename);
string createAssembly(vector<string> &input, string const &filename, int cont);

int main() {
  ifstream file;
  string query;
  int cont = 0;

  startCode("main.asm");
  
  //Reads file 
  file.open ("options.txt");
  while(getline(file, query)){
    //Assure RPN format
    cout << query;
    if (!regex_match (query, regex("^\\(.* .* .*\\)$"))){
      cout <<"Error: Invalid syntax -> '" << query << "'\n";
      break;
    }

    //Remove parentheses
    query.erase(0,1);
    query.erase(query.size()-1,1);

    //Split by blank spaces
    auto pieces = split(query);

    //Generate Assembly file
    cont++;
    string response = createAssembly(pieces, "main.asm", cont);
    if(response == ""){
      cout <<"Successfully compiled query '("+query+")'\n\n";
    } else {
      cout <<"Could not compile query '(" << query << ")'\n";
      cout <<"Error: " << response << "\n";
      //break;
    }
  };
  file.close();

  finishCode("main.asm");
  
  return 0;
}

vector<string> split(string const &input) { 
  istringstream buffer(input);
  vector<string> ret{istream_iterator<string>(buffer), {}};
  return ret;
}

bool isNumber(const string& str){
  for (char const &c : str) {
    if(c == '-' || c == '.')
      continue;
    else if (isdigit(c) == 0) 
      return false;
  }
  return true;
}

void startCode(string filename){
  ofstream assembly;
  assembly.open(filename);
  
  assembly << ".macro mostra_resultado\n";
  assembly << "	loop_operacao:\n";
  assembly << "		cbi PORTB, 1\n";
  assembly << "		cbi PORTB, 0\n";
  assembly << "		mov R16, @0\n";
  assembly << "		mov R21, R16\n";
  assembly << "		ldi R17, 100\n";
  assembly << "		cpi R30, 1\n";
  assembly << "		brne loop_espera\n";
  assembly << "		sbi PORTB, 0\n";
  assembly << "	loop_espera:\n";
  assembly << "		sbis PINB, 3\n";
  assembly << "		rjmp fim\n";
  assembly << "		sbic PINB, 2\n";
  assembly << "		rjmp loop_espera\n";
  assembly << "		cpi R17, 1\n";
  assembly << "		breq loop_espera\n";
  assembly << "	loop_bits:\n";
  assembly << "		ldi R18, 0\n";
  assembly << "		mov R19,R16\n";
  assembly << "	loop_div:\n";
  assembly << "		sub R16, R17\n";
  assembly << "		brlo fim_div\n";
  assembly << "		inc R18\n";
  assembly << "		rjmp loop_div\n";
  assembly << "	fim_div:\n";
  assembly << "		out PORTD, R18\n";
  assembly << "		pause_i_ms 220\n";
  assembly << "		mov R19, R18\n";
  assembly << "		ldi R20, 0\n";
  assembly << "	loop_resto:\n";
  assembly << "		add R20, R17\n";
  assembly << "		dec R19\n";
  assembly << "		cpi R19, 0\n";
  assembly << "		brne loop_resto\n";
  assembly << "	fim_resto:\n";
  assembly << "		sub R21, R20\n";
  assembly << "		mov R16, R21\n";
  assembly << "		cpi R17, 10\n";
  assembly << "		breq ultimo_loop\n";
  assembly << "		ldi R17, 10\n";
  assembly << "		cpi R18, 0\n";
  assembly << "		breq loop_bits\n";
  assembly << "		rjmp loop_espera\n";
  assembly << "	ultimo_loop:\n";
  assembly << "		ldi R17, 1\n";
  assembly << "		sbic PINB, 2\n";
  assembly << "		rjmp ultimo_loop\n";
  assembly << "		out PORTD, R21\n";
  assembly << "		pause_i_ms 220\n";
  assembly << "		sbi PORTB, 1\n";
  assembly << "		rjmp loop_espera\n";
  assembly << "	fim:\n";
  assembly << "		ldi R16, 0x00\n";
  assembly << "		out PORTD, R16\n";
  assembly << "   cbi PORTB, 0\n";
  assembly << ".endm\n\n\n";
  assembly << "rjmp start\n";
  assembly << ".include \"minha_lib.inc\"\n\n";
  assembly << "start:\n";
  assembly << "  ldi R16, 0xFF\n";
	assembly << "  out DDRD, R16\n";
	assembly << "  ldi R16, 0x03\n";
	assembly << "  out DDRB, R16\n";
	assembly << "  ldi R16, 0x00\n";
	assembly << "  out PORTD, R16\n";
	assembly << "  ldi R16, 0x0C\n";
	assembly << "  out PORTB, R16\n\n";
  assembly.close();
}

void finishCode(string filename){
  ofstream assembly;
  assembly.open(filename, ios_base::app);
  
  assembly << "	loop_final:\n";
  assembly << "		sbi PORTB, 1\n";
  assembly << "		rjmp loop_final\n";
  
  assembly.close();
}


string add(string n1, string n2, bool negative){
  if (n1.find('-') != string::npos) n1.erase(0,1);
  if (n2.find('-') != string::npos) n2.erase(0,1);
  string code = "";
  code += "  ldi R16, " + n1 + "\n";
  code += "  ldi R17, " + n2 + "\n";
  code += "  add R16, R17\n";
  code += negative ? "  ldi R30, 1\n" : "  ldi R30, 0\n";
  code += "  mostra_resultado R16\n";
  code += "	 pause_i_ms 220\n";
  return code;
}

string subtract(string n1, string n2, bool negative){
  if (n1.find('-') != string::npos) n1.erase(0,1);
  if (n2.find('-') != string::npos) n2.erase(0,1);
  string code = "";
  code += "  ldi R16, " + n1 + "\n";
  code += "  ldi R17, " + n2 + "\n";
  code += "  sub R16, R17\n";
  code += negative ? "  ldi R30, 1\n" : "  ldi R30, 0\n";
  code += "  mostra_resultado R16\n";
  code += "	 pause_i_ms 220\n";
  return code;
}

string multiply(string n1, string n2, bool negative){
  if (n1.find('-') != string::npos) n1.erase(0,1);
  if (n2.find('-') != string::npos) n2.erase(0,1);
  string code = "";
  code += "  ldi R16, " + n1 + "\n";
  code += "  ldi R17, " + n2 + "\n";
  code += "  mul R16, R17\n";
  code += "  mov R16, R0\n";
  code += "  mov R17, R1\n";
  code += negative ? "  ldi R30, 1\n" : "  ldi R30, 0\n";
  code += "  mostra_resultado R16\n";
  code += "	 pause_i_ms 220\n";
  return code;
}

string divide(string n1, string n2, bool negative, int cont){
  if (n1.find('-') != string::npos) n1.erase(0,1);
  if (n2.find('-') != string::npos) n2.erase(0,1);
  string code = "";
  code += "  ldi R16, " + n1 + "\n";
  code += "	 ldi R17, " + n2 + "\n";
  code += "	 ldi R18, 1\n";
  code += "	 mov R19, R16\n";
  code += "loop_div" + to_string(cont) + ":\n";
  code += "	 sub R16, R17\n";
  code += "	 cp  R16, R17\n"; 
  code += "	 brlo fim_div" + to_string(cont) + "\n";
  code += "	 inc R18\n";
  code += "	 rjmp loop_div" + to_string(cont) + "\n";
  code += "fim_div" + to_string(cont) + ":\n";
  code += negative ? "  ldi R30, 1\n" : "  ldi R30, 0\n";
  code += "  mostra_resultado R18\n";
  code += "	 pause_i_ms 220\n";
  return code;
}

string remainder(string n1, string n2, bool negative, int cont){
  if (n1.find('-') != string::npos) n1.erase(0,1);
  if (n2.find('-') != string::npos) n2.erase(0,1);
  string code = "";
  code += "  ldi R16, " + n1 + "\n";
	code += "  ldi R17, " + n2 + "\n";
	code += "  push R16\n";
	code += "  push R17\n";
	code += "	 ldi R18, 1\n";
  code += "	 mov R19, R16\n";
  code += "loop_div" + to_string(cont) + ":\n";
  code += "	 sub R16, R17\n";
  code += "	 cp  R16, R17\n"; 
  code += "	 brlo fim_div" + to_string(cont) + "\n";
  code += "	 inc R18\n";
  code += "	 rjmp loop_div" + to_string(cont) + "\n";
  code += "fim_div" + to_string(cont) + ":\n";
	code += "  pop R17\n";
	code += "  pop R16\n";
	code += "  mul R17, R18\n";
	code += "  sub R16, R0\n";
  code += negative ? "  ldi R30, 1\n" : "  ldi R30, 0\n";
  code += "  mostra_resultado R16\n";
  code += "	 pause_i_ms 220\n";
  return code;
}

string power(string n1, string n2, bool negative, int cont){
  if (n1.find('-') != string::npos) n1.erase(0,1);
  if (n2.find('-') != string::npos) n2.erase(0,1);
  string code = "";
  code += "  ldi R16, " + n1 + "\n";
  code += "	 ldi R17, " + n2 + "\n";
  code += "	 mov R20, R16\n";
  code += "loop_potencia" + to_string(cont) + ":\n";
  code += "	 cpi R17, 1\n";
  code += "	 breq fim_potencia" + to_string(cont) + "\n";
  code += "	 dec R17\n";
  code += "	 mul R16, R20\n";
  code += "	 mov R20, R0\n";
  code += "	 rjmp loop_potencia" + to_string(cont) + "\n";
  code += "fim_potencia" + to_string(cont) + ":\n";
  code += "	 mov R16, R20\n";
  code += negative ? "  ldi R30, 1\n" : "  ldi R30, 0\n";
  code += "  mostra_resultado R16\n";
  code += "	 pause_i_ms 220\n";
  return code;
}

string validateNumber(string n, string pos){
  if(!isNumber(n)) return "Non-numeric character at position "+pos+" -> '"+n+"'\n";
  else if (n.find('.') != string::npos) return "Float number at position "+pos+" -> '"+n+"'\n";
  else return "";
}

string createAssembly(vector<string> &components, string const &filename, int cont){

  //Open assembly file
  ofstream assembly;
  assembly.open(filename, ios_base::app);

  //Handle first number
  string n1 = components.front();
  string response = validateNumber(n1, "1");
  if(response != ""){
    assembly.close();
    assembly.open(filename); //clear file
    assembly.close();
    return response;
  }
  components.erase(components.begin());

  //Handle second number
  string n2 = components.front();
  response = validateNumber(n2, "2");
  if(response != ""){
    assembly.close();
    assembly.open(filename); //clear file
    assembly.close();
    return response;
  }
  components.erase(components.begin());

  //Handle arithimetical operator
  string op = components.front();
  int num1 = stoi(n1);
  int num2 = stoi(n2);
  if(op == "+"){
    if(num1 < 0){                                                           // n1           n2
      if(num2 < 0)              assembly << add(n1,n2,true) << "\n";        // neg          neg
      else if(-num1 < num2)     assembly << subtract(n2,n1,false) << "\n";  // neg          pos(maior)
      else                      assembly << subtract(n1,n2,true) << "\n";   // neg          pos(menor)
    } else if(num2 < 0){
      if(num1 < -num2)         assembly << subtract(n2,n1,true) << "\n";    // pos(menor)   neg
      else                     assembly << subtract(n1,n2,false) << "\n";   // pos(maior)   neg
    } else {
                               assembly << add(n1,n2,false) << "\n";        // pos          pos
    }   
  } else if(op == "-"){
    if(num1 < 0){                                                           // n1           n2
      if(num2 > 0)              assembly << add(n1,n2,true) << "\n";        // neg          pos
      else if(-num1 < -num2)    assembly << subtract(n2,n1,false) << "\n";  // neg(menor)   neg
      else                      assembly << subtract(n1,n2,true) << "\n";   // neg(maior)   neg
    } else if(num2 < 0){
                                assembly << add(n1,n2,false) << "\n";       // pos          neg
    } else {
      if(num1 < num2)           assembly << subtract(n2,n1,true) << "\n";   // pos(menor)   pos
      else                      assembly << subtract(n1,n2,false) << "\n";  // pos(maior)   pos
    } 
  } else if(op == "*"){
    if((num1 < 0 && num2 < 0) || (num1 > 0 && num2 > 0)){                   // n1           n2
                                assembly << multiply(n1,n2,false) << "\n";  // neg          neg
                                                                            // pos          pos
    } else {
                                assembly << multiply(n1,n2,true) << "\n";   // pos          neg
                                                                            // neg          pos
    } 
  } else if(op == "/"){
    if((num1 < 0 && num2 < 0) || (num1 > 0 && num2 > 0)){                   // n1           n2
                                assembly << divide(n1,n2,false,cont) << "\n";    // neg          neg
                                                                            // pos          pos 
    } else {
                                assembly << divide(n1,n2,true,cont) << "\n";     // pos          neg
                                                                            // neg          pos
    } 
  } else if(op == "%"){
    if((num1 < 0 && num2 < 0) || (num1 > 0 && num2 > 0)){                   // n1           n2
                                assembly << remainder(n1,n2,false,cont) << "\n"; // neg          neg
                                                                            // pos          pos
    } else {
                                assembly << remainder(n1,n2,true,cont) << "\n";  // pos          neg
                                                                            // neg          pos
    } 
  } else if(op == "**"){                                                    // n1           n2
    if(num2 < 0){                                                           // *            neg
      assembly.close();
      assembly.open(filename); //clear file
      assembly.close();
      return "Não é possível calcular resultado para potência negativa!\n";
    } else if(num1 > 0){
                                assembly << power(n1,n2,false,cont) << "\n";     // pos          pos
    } else {
      if (num1%2 == 0)          assembly << power(n1,n2,false,cont) << "\n";     // neg(par)     pos
      else                      assembly << power(n1,n2,true,cont) << "\n";      // neg(impar)   pos
    }
  } else {
    assembly.close();
    assembly.open(filename); //clear file
    assembly.close();
    return "Invalid syntax -> '" + op + "'\n";
  }

  assembly.close();
  cout << "Result = " << n1 << " " << op << " " << n2 << "\n";
  return "";
}