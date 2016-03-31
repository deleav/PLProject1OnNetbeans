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

class Value {
public:
  string mType;
  int mInt;
  float mFloat;

  Value() {
    mType = "";
    mInt = 0;
    mFloat = 0;
  } // Value()

  Value( string type, int nInt, float nFloat ) {
    mType = type;
    mInt = nInt;
    mFloat = nFloat;
  } // Value()

  float NumToFloat() {
    if ( mType == "int" )
      return mInt;
    else if ( mType == "float" )
      return mFloat;
    return 0;
  } // NumToFloat()

  void SetAllNumZero() {
    mInt = 0;
    mFloat = 0;
  } // SetAllNumZero()

  void CleanOtherType() {
    if ( mType == "int" ) {
      mFloat = 0;
    } // if
    else if ( mType == "float" ) {
      mInt = 0;
    } // else if
  } // CleanOtherType()
}; // class Value

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
  Value mValue;

  Identifier( string token, Value value ) {
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
string gErrorMessage, gErrorToken;
bool gError = false, gIsUndefinedWhenChangeLine = false;
bool gInDeclare = false;

bool NUM( Token &token, Value &value ) ;
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
} // PrintUnrecognizedToken()

void PrintUnexpectedToken( string str ) {
  cout << "Unexpected token : '" + str + "'" << endl;
} // PrintUnexpectedToken()

void PrintUndefinedIdentifier( string str ) {
  cout << "Undefined identifier : '" + str + "'" << endl;
} // PrintUndefinedIdentifier()

void PrintError() {
  if ( gErrorMessage == "Unrecognized" )
    PrintUnrecognizedToken( gErrorToken );
  else if ( gErrorMessage == "Undefined" )
    PrintUndefinedIdentifier( gErrorToken );
  else if ( gErrorMessage == "Unexpected" )
    PrintUnexpectedToken( gErrorToken );
  else if ( gErrorMessage == "Error" )
    cout << "Error" << endl;
  gErrorMessage = gErrorToken = "";
  gIsUndefinedWhenChangeLine = false;
} // PrintError()

void PrintArithAnswer( Value value ) {
  if ( value.mType == "int" )
    cout << value.mInt << endl;
  else if ( value.mType == "float" )
    cout << fixed << setprecision( 3 ) << value.mFloat << endl;
} // PrintArithAnswer()

void PrintBoolAnswer( bool tf ) {
  if ( tf )
    cout << "true" << endl;
  else
    cout << "false" << endl;
} // PrintBoolAnswer()

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
    if ( aCharToString == "." )
      dotNum--;
    if ( IsANumChar( aCharToString ) || ( aCharToString == "." && dotNum > -1 ) ) {
      aTokenString += aCharToString;
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
      gErrorMessage = "Unrecognized";
      gErrorToken = aTokenString;
      return true;
    } // if
    else if ( aTokenString == "//" ) {
      index = oneLineString.size();
      return true;
    } // else if

    Token token( aTokenString, gAllLineToken.size(), oneLineToken.size() );
    oneLineToken.push_back( token );
    return true;
  } // if

  if ( aTokenString.size() > 0 ) {
    gErrorMessage = "Unrecognized";
    gErrorToken = aTokenString;
  } // if

  return true;
} // SymbolOrRecognizedToken()

bool GetOneLineToken() {
  // PrintNowFunction( "GetOneLineToken" );
  string oneLineString, aCharToString;
  OneLineToken oneLineToken;

  GetOneLineString( oneLineString );
  for ( int i = 0 ; i < oneLineString.size() ; i++ ) {
    aCharToString = string();
    aCharToString += oneLineString[i];
    if ( IsANumChar( aCharToString ) || aCharToString == "." ) {
      GetNumToken( oneLineString, oneLineToken, i );
    } // if
    else if ( RecognizedIDTokenHead( aCharToString ) ) {
      GetIdenToken( oneLineString, oneLineToken, i );
    } // else if
    else if ( aCharToString != " " && aCharToString != "\n" && aCharToString != "\t" ) {
      if ( !SymbolOrRecognizedToken( oneLineString, oneLineToken, i ) )
        return false;
    } // else if
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
    if ( gErrorMessage == "Undefined" ) {
      gIsUndefinedWhenChangeLine = true;
      return false;
    } // if

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
//                                Reference                                   //
// /////////////////////////////////////////////////////////////////////////////

bool ArithExp( Token &token, Value &value ) ;
bool Term( Token &token, Value &value ) ;
int gParentheses = 0;

// /////////////////////////////////////////////////////////////////////////////
//                               Sub3Command                                  //
// /////////////////////////////////////////////////////////////////////////////

bool SIGN( Token &token ) {
  // PrintNowFunction( "SIGN" );
  if ( token.mToken == "+" || token.mToken == "-" )
    return true;

  return false;
} // SIGN()

bool NUM( Token &token, Value &value ) {
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

  if ( dot == 1 ) {
    value.mType = "int";
    value.mInt = atoi( token.mToken.c_str() );
  } // if
  else {
    value.mType = "float";
    value.mFloat = atof( token.mToken.c_str() );
  } // else

  value.CleanOtherType();
  return true;
} // NUM()

// /////////////////////////////////////////////////////////////////////////////
//                               Sub2Command                                  //
// /////////////////////////////////////////////////////////////////////////////


// /////////////////////////////////////////////////
//                   Caculate start               //
// /////////////////////////////////////////////////

bool CaculateEveryThing( Value value1, Token op, Value value2, Value &value ) {
  // PrintNowFunction( "CaculateEveryThing" );
  if ( value1.mType == "int" && value2.mType == "int" ) {
    if ( op.mToken == "+" )
      value.mInt = value1.mInt + value2.mInt;
    else if ( op.mToken == "-" )
      value.mInt = value1.mInt - value2.mInt;
    else if ( op.mToken == "*" )
      value.mInt = value1.mInt * value2.mInt;
    else if ( op.mToken == "/" ) {
      if ( value2.mInt == 0 && value1.mInt != 0 ) {
        gErrorMessage = "Error";
        return false;
      } // if

      value.mInt = value1.mInt / value2.mInt;
    } // else if

    return true;
  } // if
  else {
    value.mType = "float";
    if ( op.mToken == "+" )
      value.mFloat = value1.NumToFloat() + value2.NumToFloat();
    else if ( op.mToken == "-" )
      value.mFloat = value1.NumToFloat() - value2.NumToFloat();
    else if ( op.mToken == "*" )
      value.mFloat = value1.NumToFloat() * value2.NumToFloat();
    else if ( op.mToken == "/" ) {
      if ( value2.NumToFloat() == 0 && value1.NumToFloat() != 0 ) {
        gErrorMessage = "Error";
        return false;
      } // if

      value.mFloat = value1.NumToFloat() / value2.NumToFloat();
    } // else if

    value.CleanOtherType();
    return true;
  } // else

  return false;
} // CaculateEveryThing()

// /////////////////////////////////////////////////////////////////////////////
//                                 Command                                    //
// /////////////////////////////////////////////////////////////////////////////

bool IDENT( Token &token ) {
  // PrintNowFunction( "IDENT" );
  Value value;
  if ( !NUM( token, value ) && !IsTable2( token.mToken ) ) {
    // 不是數字也不是符號
    return true;
  } // if

  return false;
} // IDENT()

bool QUIT( Token &token ) {
  // PrintNowFunction( "QUIT" );
  if ( token.mToken == "quit" || token.mToken == "QUIT" )
    return true;

  return false;
} // QUIT()

// /////////////////////////////////////////////////
//                  BooleanExp start              //
// /////////////////////////////////////////////////

bool CompareBoolFloat( Value value1, Token op, Value value2 ) {
  // PrintNowFunction( "CompareBool" );
  if ( value1.mType == "float" || value2.mType == "float" ) {
    if ( ( value1.NumToFloat() - value2.NumToFloat() <= 0.0001 ) &&
         ( value1.NumToFloat() - value2.NumToFloat() >= -0.0001 ) ) {
      // difference is within 0.0001
      value1.SetAllNumZero();
      value2.SetAllNumZero();
    } // if
  } // if

  if ( op.mToken == "=" )
    return value1.NumToFloat() == value2.NumToFloat();
  else if ( op.mToken == "<>" )
    return value1.NumToFloat() != value2.NumToFloat();
  else if ( op.mToken == ">" )
    return value1.NumToFloat() > value2.NumToFloat();
  else if ( op.mToken == "<" )
    return value1.NumToFloat() < value2.NumToFloat();
  else if ( op.mToken == ">=" )
    return value1.NumToFloat() >= value2.NumToFloat();
  else if ( op.mToken == "<=" )
    return value1.NumToFloat() <= value2.NumToFloat();

  return false;
} // CompareBoolFloat()

// /////////////////////////////////////////////////
//                    Factor start                //
// /////////////////////////////////////////////////

bool Factor( Token &token, Value &value ) {
  // PrintNowFunction( "Factor" );
  if ( IDENT( token ) ) {
    Token idToken( token.mToken );
    int idIndex = -1;
    if ( !IsIDENTDefined( idToken, idIndex ) ) {
      if ( gErrorMessage == "" ) {
        gErrorToken = idToken.mToken;
        gErrorMessage = "Undefined";
      } // if
    } // if
    else
      value = gIdentifier[idIndex].mValue;

    return true;
  } // if
  else if ( SIGN( token ) ) {
    Token sign = token;
    if ( NextToken( token ) ) {
      if ( NUM( token, value ) ) {
        Value value1( "int", 0, 0 );
        return CaculateEveryThing( value1, sign, value, value );
      } // if
    } // if
  } // else if
  else if ( NUM( token, value ) ) {
    return true;
  } // else if
  else if ( token.mToken == "(" ) {
    gParentheses++;
    if ( NextToken( token ) ) {
      if ( ArithExp( token, value ) ) {
        if ( token.mToken == ")" ) {
          gParentheses--;
          return gParentheses >= 0;
        } // if
      } // if2
    } // if
  } // else if

  return false;
} // Factor()

// /////////////////////////////////////////////////
//                  ZeroOrMany start              //
// /////////////////////////////////////////////////

bool ZeroOrManyMulDiv( Token &token, Value &value ) {
  // PrintNowFunction( "ZeroOrManyMulDiv" );
  Value value1 = value;
  if ( token.mToken == "*" || token.mToken == "/" ) {
    Token op = token;
    if ( NextToken( token ) ) {
      if ( Factor( token, value ) ) {
        if ( CaculateEveryThing( value1, op, value, value ) ) {
          if ( NextToken( token ) ) {
            if ( token.mToken == ";" || token.mToken == ")" )
              return true;
            return ZeroOrManyMulDiv( token, value );
          } // if
        } // if
      } // if
    } // if
  } // if
  else if ( token.mToken == "+" || token.mToken == "-" ||
            token.mToken == ")" || token.mToken == ";" ||
            ( IsBoolOperator( token ) && !gInDeclare ) )
    return true;

  return false;
} // ZeroOrManyMulDiv()

bool ZeroOrManyAddSub( Token &token, Value &value ) {
  // PrintNowFunction( "ZeroOrManyAddSub" );
  Value value1 = value;
  if ( token.mToken == "+" || token.mToken == "-" ) {
    Token op = token;
    if ( NextToken( token ) ) {
      if ( Term( token, value ) ) {
        if ( CaculateEveryThing( value1, op, value, value ) ) {
          if ( token.mToken == ";" || token.mToken == ")" )
            return true;
          return ZeroOrManyAddSub( token, value );
        } // if
      } // if
    } // if
  } // if
  else if ( token.mToken == "*" || token.mToken == "/" ||
            token.mToken == ")" || token.mToken == ";" ||
            ( IsBoolOperator( token ) && !gInDeclare ) )
    return true;

  return false;
} // ZeroOrManyAddSub()

// /////////////////////////////////////////////////
//                     Term start                 //
// /////////////////////////////////////////////////

bool Term( Token &token, Value &value ) {
  // PrintNowFunction( "Term" );
  if ( Factor( token, value ) ) {
    if ( NextToken( token ) ) {
      return ZeroOrManyMulDiv( token, value );
    } // if
  } // if

  return false;
} // Term()

// /////////////////////////////////////////////////
//                   ArithExp start               //
// /////////////////////////////////////////////////

bool ArithExp( Token &token, Value &value ) {
  // PrintNowFunction( "ArithExp" );
  if ( Term( token, value ) ) {
    return ZeroOrManyAddSub( token, value );
  } // if

  return false;
} // ArithExp()

// /////////////////////////////////////////////////
//              NOT_ID_StartFactor start          //
// /////////////////////////////////////////////////

bool NOT_ID_StartFactor( Token &token, Value &value ) {
  // PrintNowFunction( "NOT_ID_StartFactor" );
  // 開頭不是ID的Factor
  if ( SIGN( token ) ) {
    Token sign = token;
    if ( NextToken( token ) ) {
      if ( NUM( token, value ) ) {
        Value value1( "int", 0, 0 );
        return CaculateEveryThing( value1, sign, value, value );
      } // if
    } // if
  } // if
  else if ( NUM( token, value ) ) {
    return true;
  } // else if
  else if ( token.mToken == "(" ) {
    gParentheses++;
    if ( NextToken( token ) ) {
      if ( ArithExp( token, value ) ) {
        if ( token.mToken == ")" ) {
          gParentheses--;
          return gParentheses >= 0;
        } // if
      } // if
    } // if
  } // else if

  return false;
} // NOT_ID_StartFactor()

// /////////////////////////////////////////////////
//               NOT_ID_StartTerm start           //
// /////////////////////////////////////////////////

bool NOT_ID_StartTerm( Token &token, Value &value ) {
  // PrintNowFunction( "NOT_ID_StartTerm" );
  // 開頭不是ID的Term
  if ( NOT_ID_StartFactor( token, value ) ) {
    if ( NextToken( token ) ) {
      return ZeroOrManyMulDiv( token, value );
    } // if
  } // if

  return false;
} // NOT_ID_StartTerm()

// /////////////////////////////////////////////////
//              NOT_ID_StartArithExp start        //
// /////////////////////////////////////////////////

bool NOT_ID_StartArithExp( Token &token, Value &value ) {
  // PrintNowFunction( "NOT_ID_StartArithExp" );
  // 開頭不是ID的Arith
  if ( NOT_ID_StartTerm( token, value ) ) {
    return ZeroOrManyAddSub( token, value );
  } // if

  return false;
} // NOT_ID_StartArithExp()

bool NOT_ID_StartArithExpOrBexp( Token &token, Value &value ) {
  // PrintNowFunction( "NOT_ID_StartArithExpOrBexp" );
  // 開頭不是ID的Arith or Bool
  if ( NOT_ID_StartArithExp( token, value ) ) {
    if ( token.mToken == ";" ) {
      if ( gErrorMessage == "" )
        PrintArithAnswer( value );
      return true;
    } // if
    else if ( IsBoolOperator( token ) ) {
      Value value1 = value;
      Token op = token;
      if ( NextToken( token ) ) {
        if ( ArithExp( token, value ) ) {
          if ( token.mToken == ";" ) {
            if ( gErrorMessage == "" )
              PrintBoolAnswer( CompareBoolFloat( value1, op, value ) );
            return true;
          } // if
        } // if
      } // if
    } // if
  } // if

  if ( gErrorMessage != "Error" && gErrorMessage != "Unrecognized" && !gIsUndefinedWhenChangeLine ) {
    gErrorMessage = "Unexpected";
    gErrorToken = token.mToken;
  } // if

  return false;
} // NOT_ID_StartArithExpOrBexp()

bool Sub_IDlessArithExpOrBexp( Token &token, Value &value ) {
  Value value1 = value;
  Token op = token;
  if ( token.mToken == "+" || token.mToken == "-" ) {
    if ( NextToken( token ) ) {
      if ( Term( token, value ) ) {
        if ( CaculateEveryThing( value1, op, value, value ) ) {
          return Sub_IDlessArithExpOrBexp( token, value );
        } // if
      } // if
    } // if
  } // if
  else if ( token.mToken == "*" || token.mToken == "/" ) {
    if ( NextToken( token ) ) {
      if ( Factor( token, value ) ) {
        if ( CaculateEveryThing( value1, op, value, value ) ) {
          if ( NextToken( token ) ) {
            return Sub_IDlessArithExpOrBexp( token, value );
          } // if
        } // if
      } // if
    } // if
  } // else if
  else if ( token.mToken == ";" || IsBoolOperator( token ) )
    return true;

  return false;
} // Sub_IDlessArithExpOrBexp()

bool IDlessArithExpOrBexp( Token &token, Value &value ) {
  // PrintNowFunction( "IDlessArithExpOrBexp" );
  if ( Sub_IDlessArithExpOrBexp( token, value ) ) {
    if ( token.mToken == ";" ) {
      if ( gErrorMessage == "" )
        PrintArithAnswer( value );
      return true;
    } // if
    else if ( IsBoolOperator( token ) ) {
      Value value1 = value;
      Token op = token;
      if ( NextToken( token ) ) {
        if ( ArithExp( token, value ) ) {
          if ( token.mToken == ";" ) {
            if ( gErrorMessage == "" )
              PrintBoolAnswer( CompareBoolFloat( value1, op, value ) );
            return true;
          } // if
        } // if
      } // if
    } // if
  } // if

  if ( gErrorMessage != "Error" && gErrorMessage != "Unrecognized" && !gIsUndefinedWhenChangeLine ) {
    gErrorMessage = "Unexpected";
    gErrorToken = token.mToken;
  } // if

  return false;
} // IDlessArithExpOrBexp()

void Command( string &e ) {
  // PrintNowFunction( "Command" );
  Token token;
  Value value;
  cout << "> ";
  while ( NextToken( token ) ) {
    if ( QUIT( token ) ) {
      cout << "Program exits...";
      return ;
    } // if
    else if ( IDENT( token ) ) {
      int idIndex = -1;
      Token idToken = token;
      if ( NextToken( token ) ) {
        if ( token.mToken == ":=" ) {
          if ( NextToken( token ) ) {
            gInDeclare = true;
            if ( ArithExp( token, value ) ) {
              if ( token.mToken == ";" && gErrorMessage == "" ) {
                Identifier id( idToken.mToken, value );
                IsIDENTDefined( idToken, idIndex );
                if ( idIndex == -1 )
                  gIdentifier.push_back( id );
                else
                  gIdentifier[idIndex] = id;
                PrintArithAnswer( id.mValue );
              } // if
            } // if
            else {
              if ( gErrorMessage != "Error" && gErrorMessage != "Unrecognized" &&
                   !gIsUndefinedWhenChangeLine ) {
                gErrorMessage = "Unexpected";
                gErrorToken = token.mToken;
              } // if
            } // else
          } // if

          gInDeclare = false;
        } // if
        else {
          if ( !IsIDENTDefined( idToken, idIndex ) ) {
            gErrorToken = idToken.mToken;
            gErrorMessage = "Undefined";
          } // if
          else
            value = gIdentifier[idIndex].mValue;

          if ( IDlessArithExpOrBexp( token, value ) ) {
            // do nothing
          } // else if
        } // else
      } // if
    } // else if
    else if ( NOT_ID_StartArithExpOrBexp( token, value ) ) {
      // do nothing
    } // else if

    if ( gErrorMessage != "" ) {
      gError = true;
      PrintError();
    } // if

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
