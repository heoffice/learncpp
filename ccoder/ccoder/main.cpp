//
//  main.cpp
//  ccoder
//
//  Created by hegw on 16/4/26.
//  Copyright © 2016年 hegw. All rights reserved
//
//  NEED:
//      boost_1_60_0
//      mysql-connector-c++-1.1.7
//      https://github.com/OlafvdSpek/ctemplate v2.3
//      https://github.com/jbeder/yaml-cpp/
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <string>
#include <map>
#include <stdexcept>

#include <mysql_connection.h>
#include <mysql_driver.h>

#include <cppconn/driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <boost/date_time.hpp>
#include <yaml-cpp/yaml.h>
#include <ctemplate/template.h>
const char* BigParanthesesLeft = "{{";
const char* BigParanthesesRight= "}}";
using namespace std;
void GenerateEntity(boost::scoped_ptr< sql::Connection >& con);
void GenerateEntityType(boost::scoped_ptr< sql::Connection >& con);
void GenerateEntityHandler(boost::scoped_ptr< sql::Connection >& con);
void GenerateEntityMigration(boost::scoped_ptr< sql::Connection >& con);
void GenerateEntityDatagrid(boost::scoped_ptr< sql::Connection >& con);
void GenerateEntityMessage(boost::scoped_ptr< sql::Connection >& con);
void GenerateEntityViewIndex(boost::scoped_ptr< sql::Connection >& con);
void GenerateEntityViewUpdate(boost::scoped_ptr< sql::Connection >& con);
void GenerateEntityViewView(boost::scoped_ptr< sql::Connection >& con);
void GenerateEntityController(boost::scoped_ptr< sql::Connection >& con);
void GenerateEntityRepository();
void GenerateEntityAcl();

std::string YAMLParse( const std::string& name);
std::string template_path;
std::string output_path;

std::string TypeToFormType(const std::string &type){
    string entityType ;
    if( type.find_first_of('(') != string::npos ){
        entityType = type.substr(0,type.find_first_of('('));
    }
    else
        entityType = type;
    map<string,string> mapType;
    mapType.insert(pair<string,string>("tinytext"      , "textarea"));
    mapType.insert(pair<string,string>("mediumtext"    , "textarea"));
    mapType.insert(pair<string,string>("longtext"      , "textarea"));
    mapType.insert(pair<string,string>("text"          , "textarea"));
    mapType.insert(pair<string,string>("varchar"       , "text"));
    mapType.insert(pair<string,string>("string"        , "text"));
    mapType.insert(pair<string,string>("char"          , "text"));
    mapType.insert(pair<string,string>("date"          , "oro_date"));
    mapType.insert(pair<string,string>("datetime"      , "oro_datetime"));
    mapType.insert(pair<string,string>("timestamp"     , "oro_datetime"));
    mapType.insert(pair<string,string>("time"          , "oro_datetime"));
    mapType.insert(pair<string,string>("tinyint"       , "choice"));
    mapType.insert(pair<string,string>("smallint"      , "integer"));
    mapType.insert(pair<string,string>("mediumint"     , "integer"));
    mapType.insert(pair<string,string>("int"           , "integer"));
    mapType.insert(pair<string,string>("integer"       , "integer"));
    mapType.insert(pair<string,string>("bigint"        , "integer"));
    mapType.insert(pair<string,string>("float"         , "number"));
    mapType.insert(pair<string,string>("double"        , "number"));
    mapType.insert(pair<string,string>("real"          , "number"));
    mapType.insert(pair<string,string>("decimal"       , "number"));
    mapType.insert(pair<string,string>("numeric"       , "number"));
    //以下为设置
    mapType.insert(pair<string,string>("year"          , "date"));
    mapType.insert(pair<string,string>("longblob"      , "blob"));
    mapType.insert(pair<string,string>("blob"          , "blob"));
    mapType.insert(pair<string,string>("mediumblob"    , "blob"));
    mapType.insert(pair<string,string>("tinyblob"      , "blob"));
    mapType.insert(pair<string,string>("binary"        , "blob"));
    mapType.insert(pair<string,string>("varbinary"     , "blob"));
    mapType.insert(pair<string,string>("set"           , "simple_array"));
    map<string,string>::iterator it = mapType.find(entityType);
    if( it != mapType.end() )
        return it->second;
    throw "There is no map form type: " + type;
}

std::string TypeToEntityType(const std::string &type){
    string entityType ;
    if( type.find_first_of('(') != string::npos ){
        entityType = type.substr(0,type.find_first_of('('));
    }
    else
        entityType = type;
    map<string,string> mapType;
    mapType.insert(pair<string,string>("tinyint"       , "boolean"));
    mapType.insert(pair<string,string>("smallint"      , "smallint"));
    mapType.insert(pair<string,string>("mediumint"     , "integer"));
    mapType.insert(pair<string,string>("int"           , "integer"));
    mapType.insert(pair<string,string>("integer"       , "integer"));
    mapType.insert(pair<string,string>("bigint"        , "bigint"));
    mapType.insert(pair<string,string>("tinytext"      , "text"));
    mapType.insert(pair<string,string>("mediumtext"    , "text"));
    mapType.insert(pair<string,string>("longtext"      , "text"));
    mapType.insert(pair<string,string>("text"          , "text"));
    mapType.insert(pair<string,string>("varchar"       , "string"));
    mapType.insert(pair<string,string>("string"        , "string"));
    mapType.insert(pair<string,string>("char"          , "string"));
    mapType.insert(pair<string,string>("date"          , "date"));
    mapType.insert(pair<string,string>("datetime"      , "datetime"));
    mapType.insert(pair<string,string>("timestamp"     , "datetime"));
    mapType.insert(pair<string,string>("time"          , "time"));
    mapType.insert(pair<string,string>("float"         , "float"));
    mapType.insert(pair<string,string>("double"        , "float"));
    mapType.insert(pair<string,string>("real"          , "float"));
    mapType.insert(pair<string,string>("decimal"       , "decimal"));
    mapType.insert(pair<string,string>("numeric"       , "decimal"));
    mapType.insert(pair<string,string>("year"          , "date"));
    mapType.insert(pair<string,string>("longblob"      , "blob"));
    mapType.insert(pair<string,string>("blob"          , "blob"));
    mapType.insert(pair<string,string>("mediumblob"    , "blob"));
    mapType.insert(pair<string,string>("tinyblob"      , "blob"));
    mapType.insert(pair<string,string>("binary"        , "blob"));
    mapType.insert(pair<string,string>("varbinary"     , "blob"));
    mapType.insert(pair<string,string>("set"           , "simple_array"));
    map<string,string>::iterator it = mapType.find(entityType);
    if( it != mapType.end() )
        return it->second;
    throw "There is no map type: " + type;
}

std::string FieldToEntityField(const std::string &field){
    string entityField;
    size_t i , next = 0;
    for( i = 0 ; i < field.length(); i++ ){
        if( field[i] == '_' ){
            next = i + 1;
            continue;
        }
        if( i > 0 && next == i )
            entityField += toupper(field[i]);
        else
            entityField+= field[i];
    }
    return entityField;
}

int main(int argc, const char * argv[]) {
    
    try {
        template_path   = YAMLParse("template_path");
        output_path     = YAMLParse("output_path");
        sql::Driver * driver = sql::mysql::get_driver_instance();
        std::string db_pass =YAMLParse("db_pass");
        std::string db_user =YAMLParse("db_user");
        boost::scoped_ptr< sql::Connection > con(driver->connect(YAMLParse("db_host"), db_user, db_pass));
        con->setSchema(YAMLParse("db_name"));
        GenerateEntity(con);
        GenerateEntityRepository();
        GenerateEntityAcl();
        GenerateEntityType(con);
        GenerateEntityHandler(con);
        GenerateEntityMigration(con);
        GenerateEntityDatagrid(con);
        GenerateEntityMessage(con);
        GenerateEntityViewIndex(con);
        GenerateEntityViewView(con);
        GenerateEntityViewUpdate(con);
        GenerateEntityController(con);
        
    } catch (sql::SQLException &e) {
        /*
         The MySQL Connector/C++ throws three different exceptions:
         
         - sql::MethodNotImplementedException (derived from sql::SQLException)
         - sql::InvalidArgumentException (derived from sql::SQLException)
         - sql::SQLException (derived from std::runtime_error)
         */
        cout << "# ERR: SQLException in " << __FILE__;
       // cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
        /* Use what() (derived from std::runtime_error) to fetch the error message */
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        
        return EXIT_FAILURE;
    }
    
    cout << endl;
    cout << "DONE" << endl;
    cout << endl;
    return EXIT_SUCCESS;
}

void GenerateEntityRepository(){
    
    ctemplate::TemplateDictionary dictionary("entityRespository");
    string entity(YAMLParse("entity"));
    string bundle(YAMLParse("bundle"));
    string tableName(YAMLParse("table_name"));
    dictionary.SetValue("TableName", tableName);
    dictionary.SetValue("Bundle", bundle);
    dictionary.SetValue("Entity", entity);
    std::string output;
    ctemplate::ExpandTemplate(template_path + "/entityRepository.php.tpl", ctemplate::DO_NOT_STRIP, &dictionary, &output);
    
    ofstream out( output_path + "/" + entity + "Repository.php");
    if( out.is_open() ){
        out << output;
        out.close();
    }
}

void GenerateEntityAcl(){
    
    ctemplate::TemplateDictionary dictionary("acl");
    string entity(YAMLParse("entity"));
    string bundle(YAMLParse("bundle"));
    string tableName(YAMLParse("table_name"));
    dictionary.SetValue("TableName", tableName);
    dictionary.SetValue("Bundle", bundle);
    dictionary.SetValue("Entity", entity);
    string entityLower(entity);
    entityLower[0] = tolower(entityLower[0]);
    
    dictionary.SetValue("lower", entityLower);
    std::string output;
    ctemplate::ExpandTemplate( template_path + "/acl.yml.tpl", ctemplate::DO_NOT_STRIP, &dictionary, &output);
    
    ofstream out(output_path + "/acl.yml");
    if( out.is_open() ){
        out << output;
        out.close();
    }
}

void GenerateEntityController(boost::scoped_ptr< sql::Connection > &con){
    
    boost::scoped_ptr< sql::Statement > stmt(con->createStatement());
    boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery("SHOW FULL COLUMNS FROM " + YAMLParse("table_name") ));
    ctemplate::TemplateDictionary dictionary("controller.php");
    
    while (res->next()) {
        ctemplate::TemplateDictionary *result_dictionary = dictionary.AddSectionDictionary("ONE_RESULT");
        result_dictionary->ShowSection("FIELD_SECTION");
        string field = res->getString("Field") ;
        result_dictionary->SetValue("Field", field);
        string entityField(FieldToEntityField(field)) ;
        result_dictionary->SetValue("EntityField", entityField);
    }
    string entity(YAMLParse("entity"));
    string bundle(YAMLParse("bundle"));
    string tableName(YAMLParse("table_name"));
    dictionary.SetValue("TableName", tableName);
    dictionary.SetValue("Bundle", bundle);
    dictionary.SetValue("Entity", entity);
    string bundleLower(bundle);
    bundleLower[0] = tolower(bundle[0]);
    string entityLower = entity;
    entityLower[0] = tolower(entity[0]);
    dictionary.SetValue("BundleLower", bundleLower);
    dictionary.SetValue("EntityLower", entityLower);
    
    
    std::string output;
    ctemplate::ExpandTemplate(template_path + "/Controller.php.tpl", ctemplate::DO_NOT_STRIP, &dictionary, &output);
    
    ofstream out(output_path + "/" + entity + "Controller.php");
    if( out.is_open() ){
        out << output;
        out.close();
    }
}

void GenerateEntity(boost::scoped_ptr< sql::Connection > &con){
    
    boost::scoped_ptr< sql::Statement > stmt(con->createStatement());
    boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery("SHOW FULL COLUMNS FROM " + YAMLParse("table_name") ));
    ctemplate::TemplateDictionary dictionary("entity");
    
    while (res->next()) {
        ctemplate::TemplateDictionary *result_dictionary = dictionary.AddSectionDictionary("ONE_RESULT");
        result_dictionary->ShowSection("FIELD_SECTION");
        string field = res->getString("Field") ;
        string entityFieldType = TypeToEntityType(res->getString("Type")) ;
        if( "YES" == res->getString("Null") )
            result_dictionary->ShowSection("FieldNullSection");
        result_dictionary->SetValue("Field", field);
        result_dictionary->SetValue("EntityFieldType", entityFieldType);
        string entityField(FieldToEntityField(field)) ;
        result_dictionary->SetValue("EntityField", entityField);
        string fieldMethodName (entityField);
        fieldMethodName[0] = toupper(fieldMethodName[0]);
        result_dictionary->SetValue("FieldMethodName", fieldMethodName);
        string key(res->getString("Key"));
        if( key == "PRI" )
            result_dictionary->ShowSection("FieldPrimarySection");
    }
    string entity(YAMLParse("entity"));
    string bundle(YAMLParse("bundle"));
    string tableName(YAMLParse("table_name"));
    dictionary.SetValue("TableName", tableName);
    dictionary.SetValue("Bundle", bundle);
    dictionary.SetValue("Entity", entity);
    std::string output;
    ctemplate::ExpandTemplate(template_path + "/entity.php.tpl", ctemplate::DO_NOT_STRIP, &dictionary, &output);
    
    ofstream out(output_path + "/" + entity + ".php");
    if( out.is_open() ){
        out << output;
        out.close();
    }
}
void GenerateEntityMigration(boost::scoped_ptr< sql::Connection > &con){
    
    boost::scoped_ptr< sql::Statement > stmt(con->createStatement());
    boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery("SHOW FULL COLUMNS FROM " + YAMLParse("table_name") ));
    ctemplate::TemplateDictionary dictionary("migration");
    
    while (res->next()) {
        ctemplate::TemplateDictionary *result_dictionary = dictionary.AddSectionDictionary("ONE_RESULT");
        result_dictionary->ShowSection("FIELD_SECTION");
        string field = res->getString("Field") ;
        string entityFieldType = TypeToEntityType(res->getString("Type")) ;
        if( "YES" == res->getString("Null") )
            result_dictionary->ShowSection("FieldNullSection");
        result_dictionary->SetValue("Field", field);
        result_dictionary->SetValue("EntityFieldType", entityFieldType);
        string entityField(FieldToEntityField(field)) ;
        result_dictionary->SetValue("EntityField", entityField);
        
        string fieldLength;
        string type(res->getString("Type"));
        size_t pos = type.find_first_of('(');
        
        if( pos != std::string::npos ){
            fieldLength = type.substr(pos  + 1);
            fieldLength = fieldLength.substr(0,fieldLength.find_first_of(')'));
            //cout << "fieldLength:" << fieldLength << endl;
            result_dictionary->ShowSection("FieldLengthSection");
            string fieldPrecision;
            if( fieldLength.find_first_of(',') != string::npos ){
                fieldPrecision = fieldLength.substr(fieldLength.find_first_of(',') + 1);
                fieldLength = fieldLength.substr(0,fieldLength.find_first_of(','));
                result_dictionary->ShowSection("FieldPrecisionSection");
                result_dictionary->SetValue("FieldPrecision", fieldPrecision);
            }
            result_dictionary->SetValue("FieldLength", fieldLength);
        }
        string fieldMethodName (entityField);
        fieldMethodName[0] = toupper(fieldMethodName[0]);
        result_dictionary->SetValue("FieldMethodName", fieldMethodName);
        string key(res->getString("Key"));
        if( key == "PRI" )
            result_dictionary->ShowSection("FieldPrimarySection");
    }
    //foreign key
    boost::scoped_ptr< sql::ResultSet > res1(stmt->executeQuery("select * from  INFORMATION_SCHEMA.KEY_COLUMN_USAGE where TABLE_NAME = '" + YAMLParse("table_name") +"' and `TABLE_SCHEMA`='" + YAMLParse("db_name") + "' AND `REFERENCED_TABLE_NAME` is not null ; " ));
    while (res1->next()) {
        ctemplate::TemplateDictionary *result_dictionary = dictionary.AddSectionDictionary("TWO_RESULT");
        result_dictionary->ShowSection("FIELD_SECTION");
        string columnName(res1->getString("COLUMN_NAME"));
        result_dictionary->SetValue("ColumnName", columnName);
        string refColumnName(res1->getString("REFERENCED_COLUMN_NAME"));
        result_dictionary->SetValue("RefColumnName", refColumnName);
        string refTableName(res1->getString("REFERENCED_TABLE_NAME"));
        result_dictionary->SetValue("RefTableName", refTableName);
        
    }
    
    string entity(YAMLParse("entity"));
    string bundle(YAMLParse("bundle"));
    string tableName(YAMLParse("table_name"));
    dictionary.SetValue("TableName", tableName);
    dictionary.SetValue("Bundle", bundle);
    dictionary.SetValue("Entity", entity);
    //todo: migeration version
    std::cout << "SELECT version FROM oro_migrations:" << bundle << endl;
    boost::scoped_ptr< sql::ResultSet > res2(stmt->executeQuery("SELECT version FROM oro_migrations WHERE bundle='Appcoachs" + bundle + "Bundle' " ));
    if (res2->next()) {
        string version = res2->getString("version");
        std::cout << "bundle  version:" << bundle << " " << version << endl;
        dictionary.SetValue("version",version.substr(1,version.size()));
    }
    std::string output;
    ctemplate::ExpandTemplate(template_path + "/migration.php.tpl", ctemplate::DO_NOT_STRIP, &dictionary, &output);
    
    ofstream out(output_path + "/Appcoachs" + bundle + "Bundle.php");
    if( out.is_open() ){
        out << output;
        out.close();
    }
}

void GenerateEntityViewIndex(boost::scoped_ptr< sql::Connection > &con){
    
    boost::scoped_ptr< sql::Statement > stmt(con->createStatement());
    boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery("SHOW FULL COLUMNS FROM " + YAMLParse("table_name") ));
    ctemplate::TemplateDictionary dictionary("index.html.twig");
    
    while (res->next()) {
        ctemplate::TemplateDictionary *result_dictionary = dictionary.AddSectionDictionary("ONE_RESULT");
        result_dictionary->ShowSection("FIELD_SECTION");
        string field = res->getString("Field") ;
        result_dictionary->SetValue("Field", field);
        
    }
    string entity(YAMLParse("entity"));
    string bundle(YAMLParse("bundle"));
    
    dictionary.SetValue("Bundle", bundle);
    dictionary.SetValue("Entity", entity);
    
    string bundleLower(bundle);
    bundleLower[0] = tolower(bundle[0]);
    string entityLower = entity;
    entityLower[0] = tolower(entity[0]);
    dictionary.SetValue("BundleLower", bundleLower);
    dictionary.SetValue("EntityLower", entityLower);
    dictionary.SetValue("BigParanthesesLeft", BigParanthesesLeft);
    dictionary.SetValue("BigParanthesesRight", BigParanthesesRight);
    
    std::string output;
    ctemplate::ExpandTemplate(template_path + "/index.html.twig.tpl", ctemplate::DO_NOT_STRIP, &dictionary, &output);
    
    ofstream out(output_path + "/index.html.twig");
    if( out.is_open() ){
        out << output;
        out.close();
    }
}

void GenerateEntityViewView(boost::scoped_ptr< sql::Connection > &con){
    
    boost::scoped_ptr< sql::Statement > stmt(con->createStatement());
    boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery("SHOW FULL COLUMNS FROM " + YAMLParse("table_name") ));
    ctemplate::TemplateDictionary dictionary("view.html.twig");
    
    while (res->next()) {
        ctemplate::TemplateDictionary *result_dictionary = dictionary.AddSectionDictionary("ONE_RESULT");
        result_dictionary->ShowSection("FIELD_SECTION");
        string field = res->getString("Field") ;
        result_dictionary->SetValue("Field", field);
        result_dictionary->SetValue("EntityField", FieldToEntityField(field));
        
    }
    string entity(YAMLParse("entity"));
    string bundle(YAMLParse("bundle"));
    
    dictionary.SetValue("Bundle", bundle);
    dictionary.SetValue("Entity", entity);
    
    string bundleLower(bundle);
    bundleLower[0] = tolower(bundle[0]);
    string entityLower = entity;
    entityLower[0] = tolower(entity[0]);
    dictionary.SetValue("BundleLower", bundleLower);
    dictionary.SetValue("EntityLower", entityLower);
    dictionary.SetValue("BigParanthesesLeft", BigParanthesesLeft);
    dictionary.SetValue("BigParanthesesRight", BigParanthesesRight);
    
    std::string output;
    ctemplate::ExpandTemplate(template_path + "/view.html.twig.tpl", ctemplate::DO_NOT_STRIP, &dictionary, &output);
    
    ofstream out(output_path + "/view.html.twig");
    if( out.is_open() ){
        out << output;
        out.close();
    }
}
void GenerateEntityViewUpdate(boost::scoped_ptr< sql::Connection > &con){
    
    boost::scoped_ptr< sql::Statement > stmt(con->createStatement());
    boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery("SHOW FULL COLUMNS FROM " + YAMLParse("table_name") ));
    ctemplate::TemplateDictionary dictionary("update.html.twig");
    
    while (res->next()) {
        ctemplate::TemplateDictionary *result_dictionary = dictionary.AddSectionDictionary("ONE_RESULT");
        result_dictionary->ShowSection("FIELD_SECTION");
        string field = res->getString("Field") ;
        result_dictionary->SetValue("Field", field);
        result_dictionary->SetValue("EntityField", FieldToEntityField(field));
        
    }
    string entity(YAMLParse("entity"));
    string bundle(YAMLParse("bundle"));
    
    dictionary.SetValue("Bundle", bundle);
    dictionary.SetValue("Entity", entity);
    
    string bundleLower(bundle);
    bundleLower[0] = tolower(bundle[0]);
    string entityLower = entity;
    entityLower[0] = tolower(entity[0]);
    dictionary.SetValue("BundleLower", bundleLower);
    dictionary.SetValue("EntityLower", entityLower);
    dictionary.SetValue("BigParanthesesLeft", BigParanthesesLeft);
    dictionary.SetValue("BigParanthesesRight", BigParanthesesRight);
    
    std::string output;
    ctemplate::ExpandTemplate(template_path + "/update.html.twig.tpl", ctemplate::DO_NOT_STRIP, &dictionary, &output);
    
    ofstream out(output_path + "/update.html.twig");
    if( out.is_open() ){
        out << output;
        out.close();
    }
}

void GenerateEntityMessage(boost::scoped_ptr< sql::Connection > &con){
    
    boost::scoped_ptr< sql::Statement > stmt(con->createStatement());
    boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery("SHOW FULL COLUMNS FROM " + YAMLParse("table_name") ));
    ctemplate::TemplateDictionary dictionary("message");
    
    while (res->next()) {
        ctemplate::TemplateDictionary *result_dictionary = dictionary.AddSectionDictionary("ONE_RESULT");
        result_dictionary->ShowSection("FIELD_SECTION");
        string field = res->getString("Field") ;
        result_dictionary->SetValue("Field", field);
        string label(field);
        bool nextUpper(false);
        label[0] = toupper(label[0]);
        for(string::iterator it = label.begin() ; it != label.end(); it++){
            if( *it == '_' ){
                *it = ' ';
                nextUpper = true;
            }
            else if (nextUpper) {
                *it = toupper(*it);
                nextUpper = false;
            }
        }
        result_dictionary->SetValue("FieldLabel", label);
        string entityField(FieldToEntityField(field)) ;
        result_dictionary->SetValue("EntityField", entityField);
        
        
    }
    string entity(YAMLParse("entity"));
    string bundle(YAMLParse("bundle"));
    
    dictionary.SetValue("Bundle", bundle);
    dictionary.SetValue("Entity", entity);
    
    string bundleLower(bundle);
    bundleLower[0] = tolower(bundle[0]);
    string entityLower = entity;
    entityLower[0] = tolower(entity[0]);
    dictionary.SetValue("BundleLower", bundleLower);
    dictionary.SetValue("EntityLower", entityLower);
    
    
    std::string output;
    ctemplate::ExpandTemplate(template_path + "/messages_en.yml.tpl", ctemplate::DO_NOT_STRIP, &dictionary, &output);
    
    ofstream out(output_path + "/messages_en.yml");
    if( out.is_open() ){
        out << output;
        out.close();
    }
}


/**
 *
 */
void GenerateEntityDatagrid(boost::scoped_ptr< sql::Connection > &con){
    
    boost::scoped_ptr< sql::Statement > stmt(con->createStatement());
    boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery("SHOW FULL COLUMNS FROM " + YAMLParse("table_name") ));
    ctemplate::TemplateDictionary dictionary("datagrid");
    
    while (res->next()) {
        ctemplate::TemplateDictionary *result_dictionary = dictionary.AddSectionDictionary("ONE_RESULT");
        result_dictionary->ShowSection("FIELD_SECTION");
        string field = res->getString("Field") ;
        string entityFieldType = TypeToEntityType(res->getString("Type")) ;
        if( entityFieldType == "datetime" || entityFieldType == "date" ){
            result_dictionary->ShowSection("DATETIME_SECTION");
        }
        if( "YES" == res->getString("Null") )
            result_dictionary->ShowSection("FieldNullSection");
        result_dictionary->SetValue("Field", field);
        result_dictionary->SetValue("EntityFieldType", entityFieldType);
        string entityField(FieldToEntityField(field)) ;
        result_dictionary->SetValue("EntityField", entityField);
        
        string fieldLength;
        string type(res->getString("Type"));
        size_t pos = type.find_first_of('(');
        
        if( pos != std::string::npos ){
            fieldLength = type.substr(pos  + 1);
            fieldLength = fieldLength.substr(0,fieldLength.find_first_of(')'));
            result_dictionary->ShowSection("FieldLengthSection");
            string fieldPrecision;
            if( fieldLength.find_first_of(',') != string::npos ){
                fieldPrecision = fieldLength.substr(fieldLength.find_first_of(',') + 1);
                fieldLength = fieldLength.substr(0,fieldLength.find_first_of(','));
                result_dictionary->ShowSection("FieldPrecisionSection");
                result_dictionary->SetValue("FieldPrecision", fieldPrecision);
            }
            result_dictionary->SetValue("FieldLength", fieldLength);
        }
        string fieldMethodName (entityField);
        fieldMethodName[0] = toupper(fieldMethodName[0]);
        result_dictionary->SetValue("FieldMethodName", fieldMethodName);
        string key(res->getString("Key"));
        if( key == "PRI" )
            result_dictionary->ShowSection("FieldPrimarySection");
    }
    boost::scoped_ptr< sql::Statement > stmt1(con->createStatement());
    boost::scoped_ptr< sql::ResultSet > res1(stmt->executeQuery("select * from  INFORMATION_SCHEMA.KEY_COLUMN_USAGE where TABLE_NAME = '" + YAMLParse("table_name") +"' and `TABLE_SCHEMA`='" + YAMLParse("db_name") + "' AND `REFERENCED_TABLE_NAME` is not null ; " ));
    while (res1->next()) {
        ctemplate::TemplateDictionary *result_dictionary = dictionary.AddSectionDictionary("TWO_RESULT");
        result_dictionary->ShowSection("FIELD_SECTION");
        string columnName(res1->getString("COLUMN_NAME"));
        result_dictionary->SetValue("ColumnName", columnName);
        string refColumnName(res1->getString("REFERENCED_COLUMN_NAME"));
        result_dictionary->SetValue("RefColumnName", refColumnName);
        string refTableName(res1->getString("REFERENCED_TABLE_NAME"));
        result_dictionary->SetValue("RefTableName", refTableName);
        
        result_dictionary->SetValue("RefTableNameAlias", refTableName.substr(0,3));
        
    }
    
    string entity(YAMLParse("entity"));
    string bundle(YAMLParse("bundle"));
    string tableName(YAMLParse("table_name"));
    dictionary.SetValue("TableName", tableName);
    dictionary.SetValue("Bundle", bundle);
    dictionary.SetValue("Entity", entity);
    
    string bundleLower(bundle);
    bundleLower[0] = tolower(bundle[0]);
    string entityLower = entity;
    entityLower[0] = tolower(entity[0]);
    dictionary.SetValue("BundleLower", bundleLower);
    dictionary.SetValue("EntityLower", entityLower);
    
    dictionary.SetValue("EntityAlias", entityLower.substr(0,1));

    std::string output;
    ctemplate::ExpandTemplate(template_path + "/datagrid.yml.tpl", ctemplate::DO_NOT_STRIP, &dictionary, &output);
    
    ofstream out(output_path + "/datagrid.yml");
    if( out.is_open() ){
        out << output;
        out.close();
    }
}

void GenerateEntityType(boost::scoped_ptr< sql::Connection > &con){
    
    boost::scoped_ptr< sql::Statement > stmt(con->createStatement());
    boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery("SHOW FULL COLUMNS FROM " + YAMLParse("table_name") ));
    ctemplate::TemplateDictionary dictionary("entity");
    
    while (res->next()) {
        ctemplate::TemplateDictionary *result_dictionary = dictionary.AddSectionDictionary("ONE_RESULT");
        result_dictionary->ShowSection("FIELD_SECTION");
        string field = res->getString("Field") ;
        if( "NO" == res->getString("Null") )
            result_dictionary->ShowSection("FieldRequiredSection");
        result_dictionary->SetValue("Field", field);
        string fieldFormType = TypeToFormType(res->getString("Type")) ;
        result_dictionary->SetValue("FieldFormType",fieldFormType);
        
        string entityField(FieldToEntityField(field)) ;
        result_dictionary->SetValue("EntityField", entityField);
        string fieldMethodName (entityField);
        fieldMethodName[0] = toupper(fieldMethodName[0]);
        result_dictionary->SetValue("FieldMethodName", fieldMethodName);
        string key(res->getString("Key"));
        if( key == "PRI" )
            result_dictionary->ShowSection("FieldPrimarySection");
    }
    string entity(YAMLParse("entity"));
    string bundle(YAMLParse("bundle"));
    string tableName(YAMLParse("table_name"));
    
    dictionary.SetValue("TableName", tableName);
    dictionary.SetValue("Bundle", bundle);
    dictionary.SetValue("Entity", entity);
    string bundleLower(bundle);
    bundleLower[0] = tolower(bundle[0]);
    string entityLower = entity;
    entityLower[0] = tolower(entity[0]);
    dictionary.SetValue("BundleLower", bundleLower);
    dictionary.SetValue("EntityLower", entityLower);
    
    std::string output;
    ctemplate::ExpandTemplate(template_path + "/type.php.tpl", ctemplate::DO_NOT_STRIP, &dictionary, &output);
    
    ofstream out(output_path + "/" + entity + "Type.php");
    if( out.is_open() ){
        out << output;
        out.close();
    }
}
void GenerateEntityHandler(boost::scoped_ptr< sql::Connection > &con){
    
    boost::scoped_ptr< sql::Statement > stmt(con->createStatement());
    boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery("SHOW FULL COLUMNS FROM " + YAMLParse("table_name") ));
    ctemplate::TemplateDictionary dictionary("handler");
    
    
    string entity(YAMLParse("entity"));
    string bundle(YAMLParse("bundle"));
    string tableName(YAMLParse("table_name"));
    
    dictionary.SetValue("TableName", tableName);
    dictionary.SetValue("Bundle", bundle);
    dictionary.SetValue("Entity", entity);
    string bundleLower(bundle);
    bundleLower[0] = tolower(bundle[0]);
    string entityLower = entity;
    entityLower[0] = tolower(entity[0]);
    dictionary.SetValue("BundleLower", bundleLower);
    dictionary.SetValue("EntityLower", entityLower);
    
    std::string output;
    ctemplate::ExpandTemplate(template_path + "/handler.php.tpl", ctemplate::DO_NOT_STRIP, &dictionary, &output);
    
    ofstream out(output_path + "/" + entity + "Handler.php");
    //cout << output << endl;
    if( out.is_open() ){
        out << output;
        out.close();
    }
}

std::string YAMLParse( const std::string& name){
    try{
        static YAML::Node config = YAML::LoadFile("config.yaml");
        return config[name].as<std::string>();
    }
    catch(exception ee){
        return "";
    }
}
