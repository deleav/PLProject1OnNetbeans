# include <stdlib.h>
# include <iostream>
# include <string>
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
    string strArray[] = { "+", "-", "*", "/", ";", "(", ")", ":=", "=", "<>", ">", "<", ">=", "<=", "//" };
    vector<string> table2;
    for ( int i = 0 ; i < 15 ; i++ )
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

bool NextLine( OneLineToken & oneLineToken ) {
  PrintNowFunction( "NextLine" );
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
    // PrintNowFunction( "nowColumnIndex < oneLineToken.size()" );
    token = oneLineToken[nowColumnIndex];
    return true;
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

bool ArithExp( Token &token );
bool IDENT( Token &token );

// /////////////////////////////////////////////////////////////////////////////
//                               Sub3Command                                  //
// /////////////////////////////////////////////////////////////////////////////

bool SIGN( Token &token ) {
  PrintNowFunction( "SIGN" );

  return false;
} // SIGN()

bool NUM( Token &token ) {
  PrintNowFunction( "NUM" );

  return false;
} // NUM()

// /////////////////////////////////////////////////////////////////////////////
//                               Sub2Command                                  //
// /////////////////////////////////////////////////////////////////////////////

bool Factor( Token &token ) {
  PrintNowFunction( "Factor" );
  if ( SIGN( token ) ) {
    if ( NextToken( token ) ) {
      if ( NUM( token ) ) {
        return true;
      } // if
    } // if
  } // if
  else if ( NUM( token ) ) {
    return true;
  } // else if
  else if ( IDENT( token ) ) {
    return true;
  } // else if
  else if ( token.mToken == "(" ) {
    if ( NextToken( token ) ) {
      if ( ArithExp( token ) ) {
        if ( NextToken( token ) ) {
          if ( token.mToken == ")" ) {
            return true;
          } // if
        } // if
      } // if
    } // if
  } // else if

  return false;
} // Factor()

// /////////////////////////////////////////////////////////////////////////////
//                                SubCommand                                  //
// /////////////////////////////////////////////////////////////////////////////

bool IDENT( Token &token ) {
  PrintNowFunction( "IDENT" );

  return false;
} // IDENT()

bool Term( Token &token ) {
  PrintNowFunction( "Term" );
  if ( Factor( token ) ) {
    return true;
  } // if
  else if ( Term( token ) ) {
    if ( NextToken( token ) ) {
      if ( token.mToken == "*" ) {
        if ( NextToken( token ) ) {
          if ( Factor( token ) ) {
            return true;
          } // if
        } // if
      } // if
      else if ( token.mToken == "/" ) {
        if ( NextToken( token ) ) {
          if ( Factor( token ) ) {
            return true;
          } // if
        } // if
      } // else if
    } // if
  } // else if

  return false;
} // Term()

// /////////////////////////////////////////////////////////////////////////////
//                                 Command                                    //
// /////////////////////////////////////////////////////////////////////////////

bool QUIT( Token &token ) {
  PrintNowFunction( "QUIT" );
  if ( token.mToken == "quit" || token.mToken == "QUIT" )
    return true;

  return false;
} // QUIT()

bool ArithExp( Token &token ) {
  PrintNowFunction( "ArithExp" );
  if ( Term( token ) ) {
    return true;
  } // if
  else if ( ArithExp( token ) ) {
    if ( NextToken( token ) ) {
      if ( token.mToken == "+" ) {
        if ( NextToken( token ) ) {
          if ( Term( token ) ) {
            return true;
          } // if
        } // if
      } // if
      else if ( token.mToken == "-" ) {
        if ( NextToken( token ) ) {
          if ( Term( token ) ) {
            return true;
          } // if
        } // if
      } // else if
    } // if
  } // else if

  return false;
} // ArithExp()

bool BooleanExp( Token &token ) {
  PrintNowFunction( "BooleanExp" );
  if ( ArithExp( token ) ) {
    if ( NextToken( token ) ) {
      if ( IsBoolOperator( token ) ) {
        if ( NextToken( token ) ) {
          if ( ArithExp( token ) ) {
            return true;
          } // if
        } // if
      } // if
    } // if
  } // if

  return false;
} // BooleanExp()

bool Statement( Token &token ) {
  PrintNowFunction( "Statement" );
  if ( IDENT( token ) ) {
    if ( NextToken( token ) ) {
      if ( token.mToken == ":=" ) {
        if ( NextToken( token ) ) {
          if ( ArithExp( token ) ) {
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
  while( NextToken( token ) ) {
    if ( QUIT( token ) ) {
      return false;
    } // if
    else if ( Statement( token ) ) {
      if ( NextToken( token ) ) {
        if ( token.mToken == ";" ) {
          return true;
        } // if
      } // if
    } // if
    // else if ( BooleanExp( token ) ) {
    //   if ( NextToken( token ) ) {
    //     if ( token.mToken == ";" ) {
    //       return true;
    //     } // if
    //   } // if
    // } // else if
    // else if ( ArithExp( token ) ) {
    //   if ( NextToken( token ) ) {
    //     if ( token.mToken == ";" ) {
    //       return true;
    //     } // if
    //   } // if
    // } // else if
  } // while

  e = "Something Error";
  return false;
} // Command()


int main() {
  string e;
  // GetToken();
  while( Command( e ) );
  if ( e.size() > 0 )
    cout << e;

  return 0;
} // main()
