# include <stdlib.h>
# include <iostream>
# include <string>
# include <sstream>
# include <vector>

using namespace std;

// /////////////////////////////////////////////////////////////////////////////
//                               class & struct                               //
// /////////////////////////////////////////////////////////////////////////////

class Token {
public:
  string mToken;
  int mX;
  int mY;

  Token() {
    mX = -1;
    mY = -1;
  } // Token()

  Token( string str ) {
    mToken = str;
    mX = -1;
    mY = -1;
  } // Token()
}; // class Token


class Table {
public:
  vector<string> mTable1; // instruction
  vector<string> mTable2; // operator

  Table() {
    mTable1 = GetTable1();
    mTable2 = GetTable2();
  } // Table()

private:
  vector<string> GetTable1() {
    string strArray[] = { "int", "char", "float" };
    vector<string> table1;
    for ( int i = 0 ; i < 3 ; i++ )
      table1.push_back( strArray[i] );
    return table1;
  } // GetTable1()

  vector<string> GetTable2() {
    string strArray[] = { "+", "-", "*", "/", ";", "(", ")", ":", ":=", "=", "<>", ">", "<", ">=", "<=", "//" };
    vector<string> table2;
    for ( int i = 0 ; i < 16 ; i++ )
      table2.push_back( strArray[i] );
    return table2;
  } // GetTable2()
}; // class Table


// /////////////////////////////////////////////////////////////////////////////
//                             typedef & global                               //
// /////////////////////////////////////////////////////////////////////////////
typedef vector<Token> OneLineToken;
typedef vector<OneLineToken> AllLineToken;

AllLineToken gAllLineToken;
Table gTable;

// /////////////////////////////////////////////////////////////////////////////
//                                   IsOO                                     //
// /////////////////////////////////////////////////////////////////////////////
bool IsTable1( string str ) {
  // instruction Table
  for ( int i = 0 ; i < gTable.mTable1.size() ; i++ ) {
    if ( gTable.mTable1[i] == str )
      return true;
  } // for

  return false;
} // IsTable1()

bool IsTable2( string str ) {
  // operator Table
  for ( int i = 0 ; i < gTable.mTable2.size() ; i++ ) {
    if ( gTable.mTable2[i] == str )
      return true;
  } // for

  return false;
} // IsTable2()

bool IsBoolOperator( Token &token ) {
  if ( token.mToken == "=" || token.mToken == "<>" || token.mToken == ">" ||
       token.mToken == "<" || token.mToken == ">=" || token.mToken == "<=" )
    return true;
  return false;
} // IsOperator()

// /////////////////////////////////////////////////////////////////////////////
//                                 Print                                      //
// /////////////////////////////////////////////////////////////////////////////

void PrintOneLineToken( vector<Token> oneLineToken ) {
  for ( int i = 0; i < oneLineToken.size() ; i++ )
    cout << oneLineToken[i].mToken << " ";
} // PrintOneLineToken()

void PrintNowFunction( string str ) {
  cout << "now in " << str << endl;
} // PrintNowFunction
// /////////////////////////////////////////////////////////////////////////////
//                                GetToken                                    //
// /////////////////////////////////////////////////////////////////////////////

void GetOneLineString( string &oneLineString ) {
  PrintNowFunction( "GetOneLineString" );
  char *str1 = new char[ 100 ];
  cin.getline( str1, 100 );
  oneLineString = str1;
  oneLineString += "\n";
} // GetOneLineString()

void GetOneLineToken( OneLineToken &oneLineToken ) {
  PrintNowFunction( "GetOneLineToken" );
  string oneLineString;
  GetOneLineString( oneLineString );
  cout << oneLineString;

  string oneTokenString;
  for ( int i = 0 ; i < oneLineString.size() ; i++ ) { // 找到一整行的token
    string oneChar;
    oneChar += oneLineString[i];
    if ( oneChar != " " && oneChar != "\n" && oneChar != "\t" && !IsTable2( oneChar ) ) {
      oneTokenString += oneChar;
    } // if
    else {
      if ( oneTokenString.length() > 0 ) {
        Token token( oneTokenString );
        oneLineToken.push_back( token );
        oneTokenString = string();
        // 將一個token塞進oneLineToken裡面
      } // if

      if ( IsTable2( oneChar ) ) {
        // 如果是符號的話，確定是否為 <>, >=, <=, //
        string op = oneChar;
        if ( i + 1 < oneLineString.size() ) {
          oneChar += oneLineString[i + 1];
          if ( IsTable2( oneChar ) ) {
            // 如果是 <>, >=, <=,
            // i 要++
            op = oneChar;
            i++;
          } // if
        } // if

        Token token( op );
        oneLineToken.push_back( token );
        // 將一個 operator token塞進oneLineToken裡面
      } // if

    } // else
  } // for

  gAllLineToken.push_back( oneLineToken );
  PrintOneLineToken( oneLineToken );
} // GetOneLineToken()

bool NextLine( OneLineToken &oneLineToken ) {
  PrintNowFunction( "NextLine" );
  oneLineToken = OneLineToken();
  // 因為&oneLineToken導致oneLineToken變成static，所以每次拿下一行之前要先清空
  GetOneLineToken( oneLineToken );
  if ( oneLineToken.size() > 0 )
    return true;

  return false;
} // NextLine()

bool NextToken( Token &token ) {
  PrintNowFunction( "NextToken" );
  static int nowColumnIndex = -1;
  static OneLineToken oneLineToken;
  nowColumnIndex++;
  if ( nowColumnIndex < oneLineToken.size() ) {
    PrintNowFunction( "nowColumnIndex < oneLineToken.size()" );
    token = oneLineToken[nowColumnIndex];
    if ( token.mToken != "//" ){
      // 如果不是註解的話
      return true;
    } // if
    else {
      // 如果是註解的話，就直接跳過這行剩下的token
      nowColumnIndex = oneLineToken.size() - 1;
      return NextToken( token );
    } // else
  } // if
  else {
    if ( NextLine( oneLineToken ) ) {
      nowColumnIndex = -1;
      return NextToken( token );
    } // if
  } // else

  return false;
} // NextToken()

void GetToken() {
  Token token;
  while ( NextToken( token ) && token.mToken != "quit" && token.mToken != "QUIT" ) {
    cout << token.mToken << endl;
  }

} // GetToken()

// /////////////////////////////////////////////////////////////////////////////
//                             FunctionReference                              //
// /////////////////////////////////////////////////////////////////////////////

bool ArithExp( Token &token, int &num );
bool IDENT( Token &token, int &num );
bool Term( Token &token, int &num );

// /////////////////////////////////////////////////////////////////////////////
//                               Sub3Command                                  //
// /////////////////////////////////////////////////////////////////////////////

bool SIGN( Token &token ) {
  PrintNowFunction( "SIGN" );
  if ( token.mToken == "+" || token.mToken == "-" )
    return true;

  return false;
} // SIGN()

bool NUM( Token &token, int &num ) {
  PrintNowFunction( "NUM" );
  stringstream ss;
  num = atoi( token.mToken.c_str() );
  ss << num;
  if ( ss.str() == token.mToken )
    return true;

  return false;
} // NUM()

// /////////////////////////////////////////////////////////////////////////////
//                               Sub2Command                                  //
// /////////////////////////////////////////////////////////////////////////////


// /////////////////////////////////////////////////
//                    Factor start                //
// /////////////////////////////////////////////////

bool InOrOutAParenthesis( string in_or_out ) {
  PrintNowFunction( "InOrOutAParenthesis" );
  static vector<string> parenthesesStack;
  if ( in_or_out == "in" ) {
    parenthesesStack.push_back( "(" );
    return true;
  } // if
  else if ( in_or_out == "out" ) {
    if ( parenthesesStack.size() > 0 ) {
      parenthesesStack.pop_back();
      return true;
    } // if
  } // else if

  return false;
} // InOrOutAParenthesis()

bool Factor( Token &token, int &num ) {
  PrintNowFunction( "Factor" );
  if ( SIGN( token ) ) {
    if ( NextToken( token ) ) {
      if ( NUM( token, num ) ) {
        return true;
      } // if
    } // if
  } // if
  else if ( NUM( token, num ) ) {
    return true;
  } // else if
  else if ( IDENT( token, num ) ) {
    return true;
  } // else if
  else if ( token.mToken == "(" ) {
    InOrOutAParenthesis( "in" );
    if ( NextToken( token ) ) {
      return ArithExp( token, num );
    } // if
  } // else if

  return false;
} // Factor()


// /////////////////////////////////////////////////
//                     Factor end                 //
// /////////////////////////////////////////////////

// /////////////////////////////////////////////////////////////////////////////
//                                SubCommand                                  //
// /////////////////////////////////////////////////////////////////////////////

bool IDENT( Token &token, int &num ) {
  PrintNowFunction( "IDENT" );
  if ( !NUM( token, num ) && !IsTable2( token.mToken ) ) {
    return true;
  } // if

  return false;
} // IDENT()

// /////////////////////////////////////////////////
//                     Term start                 //
// /////////////////////////////////////////////////

bool SubTerm( Token &token, int &num ) {
  PrintNowFunction( "SubTerm" );
  if ( token.mToken == ")" || token.mToken == ";" || token.mToken == "+" || token.mToken == "-" ) {
    if ( token.mToken == ")" ) { // 如果是右括號，必須要成功pop掉一個左括號才能return true
      if ( InOrOutAParenthesis( "out" ) )
        return true;
    } // if
    else
      return true;
  } // if
  else if ( token.mToken == "*" ) {
    if ( NextToken( token ) ) {
      int num1 = num;
      if ( Term( token, num ) ) {
        num = num1 * num;
        return true;
      } // if
    } // if
  } // else if
  else if ( token.mToken == "/" ) {
    if ( NextToken( token ) ) {
      int num1 = num;
      if ( Term( token, num ) ) {
        num = num1 / num;
        return true;
      } // if
    } // if
  } // else if

  return false;
} // SubTerm()

bool Term( Token &token, int &num ) {
  PrintNowFunction( "Term" );
  if ( Factor( token, num ) ) {
    if ( NextToken( token ) ) {
      return SubTerm( token, num );
    } // if
  } // if

  return false;
} // Term()

// /////////////////////////////////////////////////
//                     Term end                  //
// /////////////////////////////////////////////////

// /////////////////////////////////////////////////////////////////////////////
//                                 Command                                    //
// /////////////////////////////////////////////////////////////////////////////

bool QUIT( Token &token ) {
  PrintNowFunction( "QUIT" );
  if ( token.mToken == "quit" || token.mToken == "QUIT" )
    return true;

  return false;
} // QUIT()

// /////////////////////////////////////////////////
//                   ArithExp start               //
// /////////////////////////////////////////////////

bool SubArithExp( Token &token, int &num ) {
  PrintNowFunction( "SubArithExp" );
  if ( token.mToken == ";" || token.mToken == ")" )
    return true;
  else if ( token.mToken == "+" ) {
    if ( NextToken( token ) ) {
      int num1 = num;
      if ( ArithExp( token, num ) ) {
        num = num1 + num;
        return true;
      } // if
    } // if
  } // else if
  else if ( token.mToken == "-" ) {
    if ( NextToken( token ) ) {
      int num1 = num;
      if ( ArithExp( token, num ) ) {
        num = num1 - num;
        return true;
      } // if
    } // if
  } // else if

  return false;
} // SubArithExp()

bool ArithExp( Token &token, int &num ) {
  PrintNowFunction( "ArithExp" );
  if ( Term( token, num ) ) {
    return SubArithExp( token, num );
  } // if

  return false;
} // ArithExp()

// /////////////////////////////////////////////////
//                    ArithExp end                //
// /////////////////////////////////////////////////

bool BooleanExp( Token &token, int &num ) {
  PrintNowFunction( "BooleanExp" );
  if ( ArithExp( token, num ) ) {
    if ( NextToken( token ) ) {
      if ( IsBoolOperator( token ) ) {
        if ( NextToken( token ) ) {
          if ( ArithExp( token, num ) ) {
            return true;
          } // if
        } // if
      } // if
    } // if
  } // if

  return false;
} // BooleanExp()

bool Statement( Token &token, int &num ) {
  PrintNowFunction( "Statement" );
  if ( IDENT( token, num ) ) {
    if ( NextToken( token ) ) {
      if ( token.mToken == ":=" ) {
        if ( NextToken( token ) ) {
          if ( ArithExp( token, num ) ) {
            return true;
          } // if
        } // if
      } // if
    } // if
  } // if

  return false;
} // statement()

bool Command( string &e ) {
  PrintNowFunction( "Command" );
  Token token;
  int num;
  while( NextToken( token ) ) {
    if ( QUIT( token ) ) {
      cout << "Program exits..." << endl;
      return false;
    } // if
    // else if ( Statement( token ) ) {
    //   if ( NextToken( token ) ) {
    //     if ( token.mToken == ";" ) {
    //       return true;
    //     } // if
    //   } // if
    // } // if
    // else if ( BooleanExp( token, num ) ) {
    //   if ( NextToken( token ) ) {
    //     if ( token.mToken == ";" ) {
    //       return true;
    //     } // if
    //   } // if
    // } // else if
    else if ( ArithExp( token, num ) ) {
      cout << num << endl;
      return true;
    } // else if
  } // while

  e = "Something Error";
  return false;
} // Command()

void Test() {
  string str = "(" ;
  stringstream ss;
  int a = atoi( str.c_str() );
  ss << a;
  str = ss.str();
  cout << a;
} // Test()

int main() {
  // GetToken();;
  // Test();


  string e;
  cout << "> " << endl;
  while ( Command( e ) )
    cout << "> " << e << endl;



  return 0;
} // main()
