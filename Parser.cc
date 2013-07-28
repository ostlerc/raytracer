#include "Box.h"
#include "ConstantBackground.h"
#include "Group.h"
#include "Image.h"
#include "LambertianMaterial.h"
#include "RefractionMaterial.h"
#include "MetalMaterial.h"
#include "Parser.h"
#include "PinholeCamera.h"
#include "Disc.h"
#include "Ring.h"
#include "Plane.h"
#include "PointLight.h"
#include "Scene.h"
#include "SpecularMaterial.h"
#include "Sphere.h"
#include "Triangle.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>

using namespace std;

void Parser::throwParseException(
    string const &message ) const
{
  cerr << next_token.line_number << ":" << next_token.column_number << ": " << message << endl;
  exit( 1 );
}

void Parser::readNextToken()
{
  int state = 0;
  long long mantissa = 0;
  int exponent = 0;
  int exponent_adjustment = 0;
  bool negative_mantissa = false;
  bool negative_exponent = false;
  for ( ; ; ) {
    int character = input.get();
    switch ( state ) {
      case 0:
        next_token.line_number = line_number;
        next_token.column_number = column_number;
        if ( input.eof() ) {
          next_token.token_type = Token::end_of_file;
          return;
        } else if ( character == ' ' || character == '\t' ||
                    character == '\r' || character == '\n' )
          state = 0;
        else if ( character == '/' )
          state = 1;
        else if ( character == '+' || character == '-' ) {
          negative_mantissa = character == '-';
          state = 3;
        } else if ( character >= '0' && character <= '9' ) {
          mantissa = character - '0';
          state = 4;
        } else if ( character == '.' )
          state = 5;
        else if ( character == '"' ) {
          next_token.string_value.clear();
          state = 10;
        } else if ( (character >= 'A' && character <= 'Z') ||
                    (character >= 'a' && character <= 'z') ||
                    character == '_' ) {
          next_token.string_value = static_cast< char >( character );
          state = 12;
        } else if ( character == ',' ) {
          ++column_number;
          next_token.token_type = Token::comma;
          return;
        } else if ( character == '{' ) {
          ++column_number;
          next_token.token_type = Token::left_brace;
          return;
        } else if ( character == '}' ) {
          ++column_number;
          next_token.token_type = Token::right_brace;
          return;
        } else if ( character == '[' ) {
          ++column_number;
          next_token.token_type = Token::left_bracket;
          return;
        } else if ( character == ']' ) {
          ++column_number;
          next_token.token_type = Token::right_bracket;
          return;
        } else
          throwParseException( "Unexpected character" );
        break;
      case 1:
        if ( character == '/' )
          state = 2;
        else
          throwParseException( "Malformed comment" );
        break;
      case 2:
        if ( character == '\n' || input.eof() )
          state = 0;
        break;
      case 3:
        if ( character >= '0' && character <= '9' ) {
          mantissa = character - '0';
          state = 4;
        } else if ( character == '.' )
          state = 5;
        else
          throwParseException( "Invalid number" );
        break;
      case 4:
        if ( character >= '0' && character <= '9' )
          mantissa = mantissa * 10 + character - '0';
        else if ( character == '.' )
          state = 6;
        else if ( character == 'E' || character == 'e' )
          state = 7;
        else {
          input.putback( static_cast< char >( character ) );
          next_token.integer_value = ( static_cast< int >( mantissa ) *
                                       ( negative_mantissa ? -1 : 1 ) );
          next_token.token_type = Token::integer;
          return;
        }
        break;
      case 5:
        if ( character >= '0' && character <= '9' ) {
          mantissa = character - '0';
          exponent_adjustment = 1;
          state = 6;
        } else
          throwParseException( "Invalid number" );
        break;
      case 6:
        if ( character >= '0' && character <= '9' ) {
          mantissa = mantissa * 10 + character - '0';
          ++exponent_adjustment;
        } else if ( character == 'E' || character == 'e' )
          state = 7;
        else {
          input.putback( static_cast< char >( character ) );
          next_token.real_value = ( mantissa * ( negative_mantissa ? -1 : 1 ) *
                                    pow( 10.0, -exponent_adjustment ) );
          next_token.token_type = Token::real;
          return;
        }
        break;
      case 7:
        if ( character == '+' || character == '-' ) {
          negative_exponent = character == '-';
          state = 8;
        } else if ( character >= '0' && character <= '9' ) {
          exponent = character - '0';
          state = 9;
        } else
          throwParseException( "Invalid number" );
        break;
      case 8:
        if ( character >= '0' && character <= '9' ) {
          exponent = character - '0';
          state = 9;
        } else
          throwParseException( "Invalid number" );
        break;
      case 9:
        if ( character >= '0' && character <= '9' )
          exponent = exponent * 10 + character - '0';
        else {
          input.putback( static_cast< char >( character ) );
          next_token.real_value = ( mantissa * ( negative_mantissa ? -1 : 1 ) *
                                    pow( 10.0, ( exponent * ( negative_exponent ? -1 : 1 ) -
                                                 exponent_adjustment ) ) );
          next_token.token_type = Token::real;
          return;
        }
        break;
      case 10:
        if ( input.eof() || character == '\n' )
          throwParseException( "Unterminated string" );
        else if ( character == '\\' )
          state = 11;
        else if ( character == '"' ) {
          ++column_number;
          next_token.token_type = Token::string;
          return;
        } else
          next_token.string_value.push_back( static_cast< char >( character ) );
        break;
      case 11:
        if ( input.eof() )
          throwParseException( "Unterminated string" );
        else if ( character == '\n' )
          next_token.string_value.push_back( '\n' );
        else if ( character == '\\' )
          next_token.string_value.push_back( '\\' );
        else if ( character == '"' )
          next_token.string_value.push_back( '"' );
        else
          next_token.string_value.push_back( static_cast< char >( character ) );
        state = 10;
        break;
      case 12:
        if ( (character >= '0' && character <= '9') ||
             (character >= 'A' && character <= 'Z') ||
             (character >= 'a' && character <= 'z') ||
             character == '_' )
          next_token.string_value.push_back( static_cast< char >( character ) );
        else {
          input.putback( static_cast< char >( character ) );
          next_token.token_type = Token::string;
          return;
        }
        break;
    }
    if ( character == '\n' )
    {
      ++line_number;
      column_number = 0;
    }
    else if ( character == '\t' )
      column_number = ( column_number + 8 ) / 8 * 8;
    else
      ++column_number;
  }
}

bool Parser::peek(
  Token::type const type )
{
  bool matched = next_token.token_type == type;
  if ( matched )
    readNextToken();
  return matched;
}

bool Parser::peek(
  string const &keyword )
{
  bool matched = ( next_token.token_type == Token::string &&
                   next_token.string_value == keyword );
  if ( matched )
    readNextToken();
  return matched;
}

Parser::Token Parser::match(
  Token::type const type,
  string const &failure_message )
{
  if ( next_token.token_type != type )
    throwParseException( failure_message );
  Token current_token( next_token );
  readNextToken();
  return current_token;
}

Parser::Token Parser::match(
  string const &keyword,
  string const &failure_message )
{
  if ( next_token.token_type != Token::string ||
       next_token.string_value != keyword )
    throwParseException( failure_message );
  Token current_token( next_token );
  readNextToken();
  return current_token;
}

string Parser::parseString()
{
  Token next( match( Token::string, "Expected a string" ) );
  return next.string_value;
}

bool Parser::parseBoolean()
{
  if ( peek( "true" ) )
    return true;
  else if ( peek( "false" ) )
    return false;
  else
    throwParseException( "Expected `true' or `false'." );
  return false;
}

int Parser::parseInteger()
{
  Token next( match( Token::integer, "Expected an integer" ) );
  return next.integer_value;
}

double Parser::parseReal()
{
  if ( next_token.token_type == Token::integer ) {
    Token next( match( Token::integer, "Expected an integer or real" ) );
    return static_cast< double >( next.integer_value );
  }
  Token next( match( Token::real, "Expected an integer or real" ) );
  return next.real_value;
}

Vector const Parser::parseVector()
{
  match( Token::left_bracket, "Expected a left bracket" );
  double x = parseReal();
  match( Token::comma, "Expected a comma" );
  double y = parseReal();
  match( Token::comma, "Expected a comma" );
  double z = parseReal();
  match( Token::right_bracket, "Expected a right bracket" );
  return Vector( x, y, z );
}

Point const Parser::parsePoint()
{
  match( Token::left_bracket, "Expected a left bracket" );
  double x = parseReal();
  match( Token::comma, "Expected a comma" );
  double y = parseReal();
  match( Token::comma, "Expected a comma" );
  double z = parseReal();
  match( Token::right_bracket, "Expected a right bracket" );
  return Point( x, y, z );
}

Color const Parser::parseColor()
{
  if ( peek( Token::left_bracket ) ) {
    double r = parseReal();
    match( Token::comma, "Expected a comma" );
    double g = parseReal();
    match( Token::comma, "Expected a comma" );
    double b = parseReal();
    match( Token::right_bracket, "Expected a right bracket" );
    return Color( r, g, b );
  }
  double v = parseReal();
  return Color( v, v, v );
}

Camera *Parser::parsePinholeCamera()
{
    int time = 0;

    Animation<Point> eye;
    Animation<Point> lookat;
    Animation<Vector> up;
    double hfov = 90.0;

    if ( peek( Token::left_brace ) )
        for ( ; ; )
        {
            if ( peek( "eye" ) )
                eye.addFrame(time, parsePoint());
            else if ( peek( "lookat" ) )
                lookat.addFrame(time, parsePoint());
            else if ( peek( "up" ) )
                up.addFrame(time, parseVector());
            else if ( peek( "hfov" ) )
                hfov = parseReal();
            else if ( peek( "time" ) )
            {
                time = parseInteger();
                max_time = max(time, max_time);
            }
            else if ( peek( Token::right_brace ) )
                break;
            else
                throwParseException( "Expected `eye', `lookat', `up', `hfov' or }." );
        }

    if(eye.isEmpty() || lookat.isEmpty() || up.isEmpty())
        throwParseException( "Missing `eye', `lookat' or `up'" );

    return new PinholeCamera( eye, lookat, up, hfov );
}

Camera *Parser::parseCamera()
{
    if ( peek( "pinhole" ) )
        return parsePinholeCamera();
    throwParseException( "Expected a camera type." );
    return 0;
}

Background *Parser::parseConstantBackground()
{
  Color color( 0.0, 0.0, 0.0 );
  if ( peek( Token::left_brace ) )
    for ( ; ; )
    {
      if ( peek( "color" ) )
        color = parseColor();
      else if ( peek( Token::right_brace ) )
        break;
      else
        throwParseException( "Expected `color' or }." );
    }
  return new ConstantBackground( color );
}

Background *Parser::parseBackground()
{
    if ( peek( "constant" ) )
        return parseConstantBackground();
    throwParseException( "Expected a background type." );
    return 0;
}

Light *Parser::parsePointLight()
{
  Point position( 0.0, 0.0, 10.0 );
  Color color( 1.0, 1.0, 1.0 );
  if ( peek( Token::left_brace ) )
    for ( ; ; )
    {
      if ( peek( "position" ) )
        position = parsePoint();
      else if ( peek( "color" ) )
        color = parseColor();
      else if ( peek( Token::right_brace ) )
        break;
      else
        throwParseException( "Expected `position', `color' or }." );
    }
  return new PointLight( position, color );
}

Light *Parser::parseLight()
{
    if ( peek( "point" ) )
      return parsePointLight();
    throwParseException( "Expected a light type." );
    return 0;
}

Material *Parser::parseLambertianMaterial()
{
  Color color( 1.0, 1.0, 1.0 );
  double Kd = 0.6;
  double Ka = 0.3;
  if ( peek( Token::left_brace ) )
    for ( ; ; )
    {
      if ( peek( "color" ) )
        color = parseColor();
      else if ( peek( "Kd" ) )
        Kd = parseReal();
      else if ( peek( "Ka" ) )
        Ka = parseReal();
      else if ( peek( Token::right_brace ) )
        break;
      else
        throwParseException( "Expected `color', `Kd', `Ka' or }." );
    }
  return new LambertianMaterial( color, Kd, Ka );
}

Material *Parser::parseSpecularMaterial()
{
    int time = 0;
    Color color( 1.0, 1.0, 1.0 );
    Animation<Color> highlight;
    Animation<float> Kd;
    Animation<float> Ka;
    Animation<float> Kr;
    Animation<int> exp;

    if ( peek( Token::left_brace ) )
    {
        for ( ; ; )
        {
            if ( peek( "color" ) )
                color = parseColor();
            else if ( peek( "Kd" ) )
                Kd.addFrame(time, parseReal());
            else if ( peek( "Ka" ) )
                Ka.addFrame(time, parseReal());
            else if ( peek( "Kr" ) )
                Kr.addFrame(time, parseReal());
            else if ( peek( "exp" ) || peek( "exponent" ))
                exp.addFrame(time, parseInteger());
            else if ( peek( "highlight" ) )
                highlight.addFrame(time, parseColor());
            else if ( peek( "time" ) )
            {
                time = parseInteger();
                max_time = max(time, max_time);
            }
            else if ( peek( Token::right_brace ) )
                break;
            else
                throwParseException( "Expected `color', `Kd', `Ka', `Kr', `exp', `time' or }." );
        }
    }

    if(highlight.isEmpty())
        highlight.addFrame(0, Color(1.0, 1.0, 1.0));
    if(Kd.isEmpty())
        Kd.addFrame(0, 0.6);
    if(Ka.isEmpty())
        Ka.addFrame(0, 0.3);
    if(Kr.isEmpty())
        Kr.addFrame(0, 0.4);
    if(exp.isEmpty())
        exp.addFrame(0, 50);

    return new SpecularMaterial( color, highlight, Kd, Ka, Kr, exp );
}

Material *Parser::parseRefractionMaterial()
{
    int time = 0;
    Animation<float> eta;
    Animation<int> exp;
    Animation<Color> atten;

    if ( peek( Token::left_brace ) )
    {
        for ( ; ; )
        {
            if ( peek( "eta" ) )
                eta.addFrame(time, parseReal());
            else if ( peek( "exp" ) || peek( "exponent" ))
                exp.addFrame(time,  parseInteger());
            else if ( peek( "attenuation" ) )
                atten.addFrame(time, parseColor());
            else if ( peek( "time" ) )
            {
                time = parseInteger();
                max_time = max(time, max_time);
            }
            else if ( peek( Token::right_brace ) )
                break;
            else
                throwParseException( "Expected `eta', `exp', `attenuation', `time', or }." );
        }
    }

    if(eta.isEmpty())
        eta.addFrame(0, 1. / 1.33);
    if(exp.isEmpty())
        exp.addFrame(0, 50);
    if(atten.isEmpty())
        atten.addFrame(0, Color(1.,1.,1.));

    return new RefractionMaterial( eta, exp, atten );
}

Material *Parser::parseMetalMaterial()
{
    int time = 0;
    Animation<float> color;
    Animation<int> exp;

    if ( peek( Token::left_brace ) )
    {
        for ( ; ; )
        {
            if ( peek( "color" ) )
                color.addFrame(time, parseReal());
            else if ( peek( "exp" ) || peek( "exponent" ))
                exp.addFrame(time, parseInteger());
            else if ( peek( Token::right_brace ) )
                break;
            else if ( peek( "time" ) )
            {
                time = parseInteger();
                max_time = max(time, max_time);
            }
            else
                throwParseException( "Expected `color', `exp' or }." );
        }
    }

    if(color.isEmpty())
        color.addFrame(0, 0.0);
    if(exp.isEmpty())
        exp.addFrame(0, 50);

    return new MetalMaterial( color, exp );
}

Material *Parser::parseMaterial()
{
    if ( peek( "lambertian" ) )
      return parseLambertianMaterial();
    else if ( peek( "specular" ) || peek( "phong" ))
        return parseSpecularMaterial();
    else if ( peek( "refraction" ) || peek( "dielectric") )
        return parseRefractionMaterial();
    else if ( peek( "metal" ) )
        return parseMetalMaterial();
    else if ( next_token.token_type == Token::string )
    {
        map< string, Material * >::iterator found = defined_materials.find( parseString() );
        if ( found != defined_materials.end() )
            return ( *found ).second;
    }
    throwParseException( "Expected an material type." );
    return 0;
}

Object *Parser::parseGroupObject()
{
    Group *group = new Group();
    match( Token::left_brace, "Expected a left brace" );
    while ( !peek( Token::right_brace ) )
        group->addObject( parseObject() );
    return group;
}

Object *Parser::parsePlaneObject()
{
  Material *material = default_material;
  Vector normal( 0.0, 0.0, 1.0 );
  Point point( 0.0, 0.0, 0.0 );
  if ( peek( Token::left_brace ) )
    for ( ; ; )
    {
      if ( peek( "material" ) )
        material = parseMaterial();
      else if ( peek( "normal" ) )
        normal = parseVector();
      else if ( peek( "point" ) )
        point = parsePoint();
      else if ( peek( Token::right_brace ) )
        break;
      else
        throwParseException( "Expected `material', `point', `normal' or }." );
    }
  return new Plane( material, normal, point );
}

Object *Parser::parseSphereObject()
{
    Material *material = default_material;

    int time = 0;
    Animation<Point> center;
    Animation<double> radius;

    if ( peek( Token::left_brace ) )
    {
        for ( ; ; )
        {
            if ( peek( "material" ) )
                material = parseMaterial();
            else if ( peek( "center" ) )
                center.addFrame((double)time, parsePoint());
            else if ( peek( "radius" ) )
                radius.addFrame((double)time, parseReal());
            else if ( peek( "time" ) )
            {
                time = parseInteger();
                max_time = max(time, max_time);
            }
            else if ( peek( Token::right_brace ) )
                break;
            else
                throwParseException( "Expected `material', `center', `radius', `time' or }." );
        }
    }

    if(center.isEmpty() || radius.isEmpty())
        throwParseException( "Expected `material', `center', `radius' or }." );

    return new Sphere( material, center, radius );
}

Object *Parser::parseTriangleObject()
{
    Material *material = default_material;

    int time = 0;
    Animation<Point> p1;
    Animation<Point> p2;
    Animation<Point> p3;
    Animation<Vector> normal;

    if ( peek( Token::left_brace ) )
    {
        for ( ; ; )
        {
            if ( peek( "material" ) )
                material = parseMaterial();
            else if ( peek( "p1" ) )
                p1.addFrame((double)time, parsePoint());
            else if ( peek( "p2" ) )
                p2.addFrame((double)time, parsePoint());
            else if ( peek( "p3" ) )
                p3.addFrame((double)time, parsePoint());
            else if ( peek( "normal" ) )
            {
                Vector n(parseVector());
                n.normalize();
                normal.addFrame((double)time, n);
            }
            else if ( peek( "time" ) )
            {
                time = parseInteger();
                max_time = max(time, max_time);
            }
            else if ( peek( Token::right_brace ) )
                break;
            else
                throwParseException( "Expected `material', `p1', `p2', `p3', `normal' `time' or }." );
        }
    }

    if(p1.isEmpty() || p2.isEmpty() || p3.isEmpty())
        throwParseException( "'triangle' is missing one or more of its vertices");

    return new Triangle( material, p1, p2, p3, normal );
}

Object *Parser::parseBoxObject()
{
    Material *material = default_material;

    int time = 0;
    Animation<Point> min;
    Animation<Point> _max;

    if ( peek( Token::left_brace ) )
    {
        for ( ; ; )
        {
            if ( peek( "material" ) )
                material = parseMaterial();
            else if ( peek( "min" ) || peek( "corner1") )
                min.addFrame(time, parsePoint());
            else if ( peek( "max" ) || peek( "corner2") )
                _max.addFrame((double)time, parsePoint());
            else if ( peek( "time" ) )
            {
                time = parseInteger();
                max_time = max(time, max_time);
            }
            else if ( peek( Token::right_brace ) )
                break;
            else
                throwParseException( "Expected `material', `min', `max', `time' or }." );
        }
    }

    if(min.isEmpty() || _max.isEmpty())
        throwParseException( "'box' is missing one or more of its properties `min' or `max'.");

    return new Box( material, min, _max);
}

Object *Parser::parseRingObject()
{
    Material *material = default_material;
    Vector normal( 0.0, 0.0, 1.0 );
    double radius = 0;
    double radius2 = 0;
    Point center( 0.0, 0.0, 0.0 );
    if ( peek( Token::left_brace ) )
        for ( ; ; )
        {
            if ( peek( "material" ) )
                material = parseMaterial();
            else if ( peek( "normal" ) )
                normal = parseVector();
            else if ( peek( "radius" ) || peek( "radius1") )
                radius = parseReal();
            else if ( peek( "radius2" ) )
                radius2 = parseReal();
            else if ( peek( "center" ) )
                center = parsePoint();
            else if ( peek( Token::right_brace ) )
                break;
            else
                throwParseException( "Expected `material', `normal', `radius', `radius2', `'center' or }." );
        }

    return new Ring( material, normal, radius, radius2, center );
}

Object *Parser::parseDiscObject()
{
    Material *material = default_material;
    Vector normal( 0.0, 0.0, 1.0 );
    double radius = 0;
    Point center( 0.0, 0.0, 0.0 );
    if ( peek( Token::left_brace ) )
        for ( ; ; )
        {
            if ( peek( "material" ) )
                material = parseMaterial();
            else if ( peek( "normal" ) )
                normal = parseVector();
            else if ( peek( "radius" ) )
                radius = parseReal();
            else if ( peek( "center" ) )
                center = parsePoint();
            else if ( peek( Token::right_brace ) )
                break;
            else
                throwParseException( "Expected `material', `normal', `radius', `'center' or }." );
        }
    return new Disc( material, normal, radius, center );
}


Object *Parser::parseObject()
{
    if ( peek( "group" ) )
        return parseGroupObject();
    else if ( peek( "plane" ) )
        return parsePlaneObject();
    else if ( peek( "sphere" ) )
        return parseSphereObject();
    else if ( peek( "triangle" ) )
        return parseTriangleObject();
    else if ( peek( "box" ) )
        return parseBoxObject();
    else if ( peek( "disc" ) )
        return parseDiscObject();
    else if ( peek( "ring" ) )
        return parseRingObject();
    else if ( next_token.token_type == Token::string )
    {
        map< string, Object * >::iterator found = defined_objects.find( parseString() );
        if ( found != defined_objects.end() )
            return ( *found ).second;
    }
    throwParseException( "Expected an object type." );
    return 0;
}

Parser::Parser(
  istream &input )
  : input( input ),
    line_number( 1 ),
    column_number( 0 ),
    default_material( new LambertianMaterial( Color( 1.0, 1.0, 1.0 ), 0.6, 0.3 ) ),
    max_time(0)
{
  readNextToken();
}

Scene *Parser::parseScene(
  string &filename )
{
  filename = "image.ppm";
  int xres = 512;
  int yres = 512;
  Scene *scene = new Scene();
  for ( ; ; ) {
    if ( peek( "filename" ) )
      filename = parseString();
    else if ( peek( "xres" ) )
      xres = parseInteger();
    else if ( peek( "yres" ) )
      yres = parseInteger();
    else if ( peek( "maxraydepth" ) )
      scene->setMaxRayDepth( parseInteger() );
    else if ( peek( "samplesize" ) )
        scene->setSampleSize( parseInteger() );
    else if ( peek( "minattenuation" ) )
      scene->setMinAttenuation( parseReal() );
    else if ( peek( "camera" ) )
      scene->setCamera( parseCamera() );
    else if ( peek( "background" ) )
      scene->setBackground( parseBackground() );
    else if ( peek( "ambient" ) )
      scene->setAmbient( parseColor() );
    else if ( peek( "light" ) )
      scene->addLight( parseLight() );
    else if ( peek( "scene" ) )
      scene->setObject( parseObject() );
    else if ( peek( "define" ) ) {
      if ( peek( "material" ) ) {
        string name( parseString() );
        defined_materials.insert( pair< string, Material * >( name, parseMaterial() ) );
      } else if ( peek( "object" ) ) {
        string name( parseString() );
        defined_objects.insert( pair< string, Object * >( name, parseObject() ) );
      } else
        throwParseException( "Expected `material', or `object'" );
    }
    else if ( peek( Token::end_of_file ) )
        break;
    else
        throwParseException( "Expected `filename', `xres', `yres', `maxraydepth', `sampleSize', `minattenuation', "
                             "`camera', `background', `ambient', `light', `scene', or `define'." );
  }

  scene->setImage( new Image( xres, yres ) );
  return scene;
}
