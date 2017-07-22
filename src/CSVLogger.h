#pragma once

#include <vector>
#include <string>

enum FieldType {
    Char_Type,
    String_Type,
    Int_Type,
    Float_Type,
    Double_Type,
    Type_Count
};

struct tagName {
    const char* tag; 
    FieldType   type;
    int         columnID;
};

struct valueEntry {
    const char*             fieldName;
    FieldType               type;
    union {
        unsigned char       charValue;
        const char*         stringValue;
        unsigned long long  intValue;
        double              DoubleValue;
        float               FloatValue;
    };
};

typedef std::vector<valueEntry*> entryValues;

class CSVLogger
{
public:
    CSVLogger();
    ~CSVLogger();

    bool dumpData(const char* filename = "outfile.csv");
    void addFieldValue (const char* FieldName, void * value);
    bool addTag(const char* FieldName, FieldType type);

private:
    void outputData(FILE *fp,FieldType type, void *value);
    FieldType findTagType (const char* FieldName);
    int findTagID (const char* FieldName);
    void cleanup();

    int                             columnIdCount;
    bool                            firstValue;
    
    std::vector<tagName*>           tags;
    std::vector<entryValues*>       columns;
};





