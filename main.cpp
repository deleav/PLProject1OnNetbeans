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
//                                 Command                                    //
// /////////////////////////////////////////////////////////////////////////////

bool AirthExp() {

  return false;
} // AirthExp()

bool BooleanExp() {

  return false;
} // BooleanExp()

bool Statement() {

  return false;
} // statement()

bool QUIT() {

  return false;
} // QUIT()

bool Command() {
  if ( Statement() ) {
    return true;
  } // if
  else if ( BooleanExp() ) {
    return true;
  } // else if
  else if ( AirthExp() ) {
    return true;
  } // else if

  return false;
} // Command()


int main() {
  GetToken();


  return 0;
} // main()
