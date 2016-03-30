# include <stdlib.h>
# include <iostream>
# include <string>
# include <sstream>
# include <vector>
# include <iomanip>

using namespace std;

static int uTestNum = -1;

// /////////////////////////////////////////////////////////////////////////////
//                               class & struct                               //
// /////////////////////////////////////////////////////////////////////////////

class Token {
public:
  string mToken;
  int mX;
  int mY;
  bool mIsRecord;

  Token() {
    mX = -1;
    mY = -1;
    mIsRecord = false;
  } // Token()

  Token( string str ) {
    mToken = str;
    mX = -1;
    mY = -1;
    mIsRecord = false;
  } // Token()

  Token( string str, int x, int y ) {
    mToken = str;
    mX = x;
    mY = y;
    mIsRecord = false;
  } // Token()
}; // class Token

class Identifier {
public:
  string mToken;
  float mValue;

  Identifier() {
    mValue = 0;
  } // Identifier()

  Identifier( string token, float value ) {
    mToken = token;
    mValue = value;
  } // Identifier()
}; // class Identifier

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
    string strArray[] = { "int", "char", "int" };
    vector<string> table1;
    for ( int i = 0 ; i < 3 ; i++ )
      table1.push_back( strArray[i] );
    return table1;
  } // GetTable1()

  vector<string> GetTable2() {
    string strArray[] = { "+", "-", "*", "/", ";", "(", ")", ":",
                          ":=", "=", "<>", ">", "<", ">=", "<=", "//" };
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

OneLineToken gHeadOfStatement;
AllLineToken gAllLineToken;
vector<Identifier> gIdentifier;
Table gTable;
string gErrorMessage;
bool gError = false;
bool gJump = false;

bool NUM( Token &token, float &num ) ;
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

bool IsANumChar( string str ) {
  if ( ( str[0] - '0' >= 0 ) && ( str[0] - '0' <= 9 ) )
    return true;

  return false;
} // IsANumChar()

bool RecognizedIDTokenHead( string str ) {
  // 合法的IDENT開頭
  if ( ( str[0] >= 65 && str[0] <= 90 ) ||
       ( str[0] >= 97 && str[0] <= 122 ) || str[0] == '_' )
    return true;

  return false;
} // RecognizedIDTokenHead()

bool IsBoolOperator( Token &token ) {
  if ( token.mToken == "=" || token.mToken == "<>" || token.mToken == ">" ||
       token.mToken == "<" || token.mToken == ">=" || token.mToken == "<=" )
    return true;
  return false;
} // IsBoolOperator()

bool IsIDENTDefined( Token token, int &identIndex ) {
  for ( int i = 0 ; i < gIdentifier.size() ; i++ )
    if ( gIdentifier[i].mToken == token.mToken ) {
      identIndex = i;
      return true;
    } // if

  return false;
} // IsIDENTDefined()

// /////////////////////////////////////////////////////////////////////////////
//                                 Print                                      //
// /////////////////////////////////////////////////////////////////////////////

void PrintOneLineToken( vector<Token> oneLineToken ) {
  for ( int i = 0; i < oneLineToken.size() ; i++ )
    cout << oneLineToken[i].mToken << " ";
} // PrintOneLineToken()

void PrintNowFunction( string str ) {
  cout << "now in " << str << endl;
} // PrintNowFunction()

void PrintUnrecognizedToken( string str ) {
  cout << "Unrecognized token with first char : '" + str + "'" << endl;
  cout << "> ";
} // PrintUnrecognizedToken()

void PrintUnexpectedToken( string str ) {
  cout << "Unexpected token : '" + str + "'" << endl;
} // PrintUnexpectedToken()

void PrintUndefinedIdentifier( string str ) {
  cout << "Undefined identifier : '" + str + "'" << endl;
} // PrintUndefinedIdentifier()

void PrintError( string str ) {
  if ( gErrorMessage == "Undefined" )
    PrintUndefinedIdentifier( str );
  else if ( gErrorMessage == "Unexpected" )
    PrintUnexpectedToken( str );
} // PrintError()
// /////////////////////////////////////////////////////////////////////////////
//                                GetToken                                    //
// /////////////////////////////////////////////////////////////////////////////

void GetOneLineString( string &oneLineString ) {
  // // PrintNowFunction( "GetOneLineString" );
  char *str1 = new char[ 100 ];
  cin.getline( str1, 100 );
  oneLineString = str1;
  oneLineString += "\n";
} // GetOneLineString()

bool GetNumToken( string oneLineString, OneLineToken &oneLineToken, int &index ) {
  // PrintNowFunction( "GetNumToken" );
  string aCharToString, aTokenString;
  int dotNum = 1;
  for ( ; index < oneLineString.size() ; index++ ) {
    aCharToString = string();
    aCharToString += oneLineString[index];
    if ( IsANumChar( aCharToString ) || ( aCharToString == "." && dotNum > 0 ) ) {
      aTokenString += aCharToString;
      if ( aCharToString == "." )
        dotNum--;
    } // if
    else {
      // 所有不是數字都回傳
      index--;
      Token token( aTokenString, gAllLineToken.size(), oneLineToken.size() );
      oneLineToken.push_back( token );
      return true;
    } // else
  } // for

  cout << "GetNumToken Error" << endl;
  return false;
} // GetNumToken()

bool GetIdenToken( string oneLineString, OneLineToken &oneLineToken, int &index ) {
  // PrintNowFunction( "GetIdenToken" );
  string aCharToString, aTokenString;
  for ( ; index < oneLineString.size() ; index++ ) {
    aCharToString = string();
    aCharToString += oneLineString[index];
    if ( IsANumChar( aCharToString ) || RecognizedIDTokenHead( aCharToString ) ) {
      aTokenString += aCharToString;
    } // if
    else {
      // 不是數字或英文貨底線
      index--;
      Token token( aTokenString, gAllLineToken.size(), oneLineToken.size() );
      oneLineToken.push_back( token );
      return true;
    } // else
  } // for

  cout << "GetIdenToken Error" << endl;
  return false;
} // GetIdenToken()

bool SymbolOrRecognizedToken( string oneLineString, OneLineToken &oneLineToken, int &index ) {
  // PrintNowFunction( "SymbolOrRecognizedToken" );
  string aCharToString, aTokenString;
  aCharToString = string();
  aCharToString += oneLineString[index];
  if ( IsTable2( aCharToString ) ) {
    aTokenString += aCharToString;
    index++;
    if ( index < oneLineString.size() ) {
      aCharToString += oneLineString[index];
      if ( IsTable2( aCharToString ) ) {
        aTokenString = aCharToString;
      } // if
      else
        index--;
    } // if

    if ( aTokenString == ":" ) {
      // 回報錯誤
      PrintUnrecognizedToken( aTokenString );
      return false;
    } // if
    else if ( aTokenString == "//" ) {
      index = oneLineString.size();
      return true;
    } // else if

    Token token( aTokenString, gAllLineToken.size(), oneLineToken.size() );
    oneLineToken.push_back( token );
    return true;
  } // if

  PrintUnrecognizedToken( aCharToString );
  return false;
} // SymbolOrRecognizedToken()

bool GetOneLineToken() {
  // PrintNowFunction( "GetOneLineToken" );
  string oneLineString, aCharToString;
  OneLineToken oneLineToken;

  GetOneLineString( oneLineString );
  for ( int i = 0 ; i < oneLineString.size() ; i++ ) {
    aCharToString = string();
    aCharToString += oneLineString[i];
    if ( IsANumChar( aCharToString ) ) {
      GetNumToken( oneLineString, oneLineToken, i );
    } // if
    else if ( RecognizedIDTokenHead( aCharToString ) ) {
      GetIdenToken( oneLineString, oneLineToken, i );
    } // else if
    else if ( aCharToString != " " && aCharToString != "\n" && aCharToString != "\t" ) {
      if ( !SymbolOrRecognizedToken( oneLineString, oneLineToken, i ) )
        return false;
    } // else if

    if ( oneLineToken.size() > 0 &&
         ( oneLineToken.back().mToken == "quit" || oneLineToken.back().mToken == "QUIT" ) ) {
      gAllLineToken.push_back( oneLineToken );
      return true;
    } // if
  } // for

  // PrintOneLineToken( oneLineToken );
  gAllLineToken.push_back( oneLineToken );
  return true;
} // GetOneLineToken()

bool NextLine( OneLineToken &oneLineToken, int &nowLineIndex ) {
  // PrintNowFunction( "NextLine" );
  nowLineIndex++;
  if ( nowLineIndex < gAllLineToken.size() ) {
    oneLineToken = gAllLineToken[nowLineIndex];
    return true;
  } // if

  GetOneLineToken();
  nowLineIndex--;
  return NextLine( oneLineToken, nowLineIndex );
} // NextLine()

bool NextToken( Token &token ) {
  // PrintNowFunction( "NextToken" );
  static int nowLineIndex = -1;
  static int nowColumnIndex = -1;
  static OneLineToken oneLineToken;
  nowColumnIndex++;

  if ( gJump ) {
    // 放在 nowColumnIndex++ 之後就不用 -1
    nowLineIndex = gHeadOfStatement.back().mX - 1;
    nowColumnIndex = gHeadOfStatement.back().mY;
    NextLine( oneLineToken, nowLineIndex );
    gJump = false;
  } // if

  if ( gError ) {
    // 如果這行發生錯誤直接跳下一行
    nowColumnIndex = oneLineToken.size();
    gError = false;
  } // if

  if ( nowColumnIndex < oneLineToken.size() ) {
    // // PrintNowFunction( "nowColumnIndex < oneLineToken.size()" );
    token = oneLineToken[nowColumnIndex];
    if ( token.mToken != "//" ) {
      // 如果不是註解的話
      // cout << token.mToken << endl;
      return true;
    } // if
    else {
      // 如果是註解的話，就直接跳過這行剩下的token
      nowColumnIndex = oneLineToken.size() - 1;
      return NextToken( token );
    } // else
  } // if
  else {
    if ( NextLine( oneLineToken, nowLineIndex ) ) {
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
  } // while

} // GetToken()

// /////////////////////////////////////////////////////////////////////////////
//                             FunctionReference                              //
// /////////////////////////////////////////////////////////////////////////////

bool ArithExp( Token &token, float &num ) ;
bool IDENT( Token &token, float &num ) ;
bool Term( Token &token, float &num ) ;

// /////////////////////////////////////////////////////////////////////////////
//                               Sub3Command                                  //
// /////////////////////////////////////////////////////////////////////////////

bool SIGN( Token &token ) {
  // // PrintNowFunction( "SIGN" );
  if ( token.mToken == "+" || token.mToken == "-" )
    return true;

  return false;
} // SIGN()

bool NUM( Token &token, float &num ) {
  // PrintNowFunction( "NUM" );
  int dot = 1;
  for ( int i = 0 ; i < token.mToken.size() ; i++ ) {
    if ( token.mToken[i] < 48 || token.mToken[i] > 57 ) {
      if ( token.mToken[i] == '.' ) {
        dot--;
        // 不能有第二個點
        if ( dot < 0 )
          return false;
      } // if
      else { // 不是dot也不是數字
        return false;
      } // else
    } // if
  } // for

  if ( dot < 1 ) {
    if ( token.mToken.size() > 0 && token.mToken[0] == '.' )
      return false;
    // 第一個不能是點
  } // if

  num = atof( token.mToken.c_str() );
  // 暫時先轉成integer
  return true;
} // NUM()

// /////////////////////////////////////////////////////////////////////////////
//                               Sub2Command                                  //
// /////////////////////////////////////////////////////////////////////////////


// /////////////////////////////////////////////////
//                   Caculate start               //
// /////////////////////////////////////////////////

bool CaculateEveryThing( Token num1Token, Token op, Token num2Token, Token &sumToken ) {
  float num1DotPos = -1, num2DotPos = -1;
  for ( int i = 0 ; i < num1Token.mToken.size() ; i++ )
    if ( num1Token.mToken[i] == '.' )
      num1DotPos = i;
  for ( int i = 0 ; i < num2Token.mToken.size() ; i++ )
    if ( num2Token.mToken[i] == '.' )
      num2DotPos = i;
  // 先找到算術的基準點
  // if ( num1DotPos == -1 )
  //   num1DotPos = num1Token.mToken.size() - 1;
  // if ( num2DotPos == -1 )
  //   num2DotPos = num2Token.mToken.size() - 1;

  float num1 = 0, num2 = 0, sum = 0;
  stringstream ss;
  num1 = atof( num1Token.mToken.c_str() );
  num2 = atof( num2Token.mToken.c_str() );
  if ( op.mToken == "+" ) {
    sum = num1 + num2;
    ss << sum;
    ss >> sumToken.mToken;
    return true;
  } // if
  else if ( op.mToken == "-" ) {
    sum = num1 - num2;
    ss << sum;
    ss >> sumToken.mToken;
    return true;
  } // else if
  else if ( op.mToken == "*" ) {
    sum = num1 * num2;
    ss << sum;
    ss >> sumToken.mToken;
    return true;
  } // else if
  else if ( op.mToken == "/" ) {
    if ( num2 == 0 ) {
      if ( num1 == 0 ) {
        sum = num1 / num2;
        ss << sum;
        ss >> sumToken.mToken;
        return true;
      } // if
    } // if
    else {
      sum = num1 / num2;
      ss << sum;
      ss >> sumToken.mToken;
      return true;
    } // else
  } // else if

  return false;
} // CaculateEveryThing()

// /////////////////////////////////////////////////
//                    Caculate end                //
// /////////////////////////////////////////////////

// /////////////////////////////////////////////////
//                    Factor start                //
// /////////////////////////////////////////////////

vector<string> gParenthesesStack;

bool InOrOutAParenthesis( string in_or_out ) {
  // // PrintNowFunction( "InOrOutAParenthesis" );
  if ( in_or_out == "in" ) {
    gParenthesesStack.push_back( "(" );
    return true;
  } // if
  else if ( in_or_out == "out" ) {
    if ( gParenthesesStack.size() > 0 ) {
      gParenthesesStack.pop_back();
      return true;
    } // if
  } // else if

  return false;
} // InOrOutAParenthesis()

bool Factor( Token &token, float &num ) {
  // PrintNowFunction( "Factor" );
  if ( SIGN( token ) ) {
    Token sign = token;
    if ( NextToken( token ) ) {
      if ( NUM( token, num ) ) {
        if ( sign.mToken == "-" )
          num = 0 - num;
        return true;
      } // if
    } // if
  } // if
  else if ( NUM( token, num ) ) {
    return true;
  } // else if
  else if ( IDENT( token, num ) ) {
    // 如果他是一個 IDENT，就檢查他是否有宣告過
    int identIndex;
    if ( IsIDENTDefined( token, identIndex ) ) {
      num = gIdentifier[identIndex].mValue;
      return true;
    } // if
    else {
      gErrorMessage = "Undefined";
    } // else
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

bool IDENT( Token &token, float &num ) {
  // PrintNowFunction( "IDENT" );
  if ( !NUM( token, num ) && !IsTable2( token.mToken ) ) {
    // 不是數字也不是符號
    return true;
  } // if

  return false;
} // IDENT()

// /////////////////////////////////////////////////
//                     Term start                 //
// /////////////////////////////////////////////////

bool SubTerm( Token &token, float &num ) {
  // PrintNowFunction( "SubTerm" );
  if ( token.mToken == ")" || token.mToken == ";" || token.mToken == "+" || token.mToken == "-" ||
       IsBoolOperator( token ) ) {
    if ( token.mToken == ")" ) { // 如果是右括號，必須要成功pop掉一個左括號才能return true
      if ( InOrOutAParenthesis( "out" ) )
        return true;
    } // if
    else {
      if ( token.mToken == ";" && gParenthesesStack.size() > 0 )
        return false;
      return true;
    } // else
  } // if
  else if ( token.mToken == "*" ) {
    if ( NextToken( token ) ) {
      float num1 = num;
      if ( Term( token, num ) ) {
        num = num1 * num;
        return true;
      } // if
    } // if
  } // else if
  else if ( token.mToken == "/" ) {
    if ( NextToken( token ) ) {
      float num1 = num;
      if ( Term( token, num ) ) {
        if ( num != 0 ) {
          num = num1 / num;
          return true;
        } // if
        else {
          // num == 0
          if ( num1 == 0 ) {
            num = 0;
            return true;
          } // if
          else {
            cout << "Error" << endl;
          } // else
        } // else
      } // if
    } // if
  } // else if

  return false;
} // SubTerm()

bool Term( Token &token, float &num ) {
  // PrintNowFunction( "Term" );
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
  // // PrintNowFunction( "QUIT" );
  if ( token.mToken == "quit" || token.mToken == "QUIT" )
    return true;

  return false;
} // QUIT()

// /////////////////////////////////////////////////
//                   ArithExp start               //
// /////////////////////////////////////////////////


bool SubArithExp( Token &token, float &num ) {
  // PrintNowFunction( "SubArithExp" );
  if ( token.mToken == ";" || token.mToken == ")" || IsBoolOperator( token ) ) {
    if ( gErrorMessage.size() == 0 )
      return true;
  } // if
  else if ( token.mToken == "+" ) {
    if ( NextToken( token ) ) {
      float num1 = num;
      if ( Term( token, num ) ) {
        num = num1 + num;
        return true;
      } // if
    } // if
  } // else if
  else if ( token.mToken == "-" ) {
    if ( NextToken( token ) ) {
      float num1 = num;
      if ( Term( token, num ) ) {
        num = num1 - num;
        return true;
      } // if
    } // if
  } // else if

  return false;
} // SubArithExp()

bool ArithExp( Token &token, float &num ) {
  // PrintNowFunction( "ArithExp" );
  if ( Term( token, num ) ) {
    return SubArithExp( token, num );
  } // if

  return false;
} // ArithExp()

// /////////////////////////////////////////////////
//                    ArithExp end                //
// /////////////////////////////////////////////////

// /////////////////////////////////////////////////
//                  BooleanExp start              //
// /////////////////////////////////////////////////

bool CompareBool( float num1, Token op, float num2 ) {
  // PrintNowFunction( "CompareBool" );
  if ( ( num1 - num2 <= 0.0001 ) && ( num1 - num2 >= -0.0001 ) ) {
    // difference is within 0.0001
    num1 = num2 = 0;
  } // if

  if ( op.mToken == "=" )
    return num1 == num2;
  else if ( op.mToken == "<>" )
    return num1 != num2;
  else if ( op.mToken == ">" )
    return num1 > num2;
  else if ( op.mToken == "<" )
    return num1 < num2;
  else if ( op.mToken == ">=" )
    return num1 >= num2;
  else if ( op.mToken == "<=" )
    return num1 <= num2;

  return false;
} // CompareBool()

bool BooleanExp( Token &token, float &num ) {
  // PrintNowFunction( "BooleanExp" );
  if ( ArithExp( token, num ) ) {
    if ( token.mToken == ";" ) {
      cout << num << endl;
      return true;
    } // if
    else if ( IsBoolOperator( token ) ) {
      Token op = token;
      float num1 = num;
      if ( NextToken( token ) ) {
        if ( ArithExp( token, num ) ) {
          if ( token.mToken == ";" ) {
            if ( CompareBool( num1, op, num ) )
              cout << "true" << endl;
            else
              cout << "false" << endl;
            return true;
          } // if
        } // if
      } // if
    } // else if
  } // if

  if ( gErrorMessage == "" )
    gErrorMessage = "Unexpected";
  return false;
} // BooleanExp()

// /////////////////////////////////////////////////
//                   BooleanExp end               //
// /////////////////////////////////////////////////

bool Statement( Token &token, float &num ) {
  // PrintNowFunction( "Statement" );
  if ( IDENT( token, num ) ) {
    Token token1 = token;
    if ( NextToken( token ) ) {
      if ( token.mToken == ":=" ) {
        if ( NextToken( token ) ) {
          if ( ArithExp( token, num ) ) {
            float num1 = num;
            if ( token.mToken == ";" ) {
              int identIndex = -1;
              if ( IsIDENTDefined( token1, identIndex ) ) {
                gIdentifier[identIndex].mValue = num1;
                cout << gIdentifier[identIndex].mValue << endl;
              } // if
              else {
                Identifier ident;
                ident = Identifier( token1.mToken, num1 );
                gIdentifier.push_back( ident );
                cout << ident.mValue << endl;
              } // else

              return true;
            } // if
          } // if
        } // if
      } // if
      else {
        // 發現不是宣告的時候就強制跳到這個 Statement 的第一個 token
        gJump = true;
        NextToken( token );
      } // else
    } // if
  } // if

  return false;
} // Statement()

void Command( string &e ) {
  // // PrintNowFunction( "Command" );
  Token token;
  float num;
  cout << "> ";
  while ( NextToken( token ) ) {
    if ( !token.mIsRecord ) {
      gHeadOfStatement.push_back( token );
      token.mIsRecord = true;
    } // if

    if ( token.mToken != ";" ) {
      if ( QUIT( token ) ) {
        cout << "Program exits..." << endl;
        return ;
      } // if
      else if ( Statement( token, num ) ) {
        // do nothing
      } // if
      else if ( BooleanExp( token, num ) ) {
        // do nothing
      } // else if
      else {
        PrintError( token.mToken );
        gErrorMessage = "";
        gError = true;
      } // else
    } // if
    else {
      gErrorMessage = "Unexpected";
      PrintError( token.mToken );
      gErrorMessage = "";
      gError = true;
    } // else

    cout << "> ";
  } // while
} // Command()

void Test() {
  string str = "(" ;
  stringstream ss;
  int a = atof( str.c_str() );
  ss << a;
  str = ss.str();
  cout << a;
} // Test()

int main() {
  // GetToken();
  // Test();

  string e, testNum;
  char *str1 = new char[ 100 ];
  cin.getline( str1, 100 );
  testNum = str1;
  uTestNum  = atof( testNum.c_str() );
  cout << "Program starts..." << endl;

  Command( e );


  return 0;
} // main()
