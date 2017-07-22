// Output Logging.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>
#include "CSVLogger.h"



bool CSVLogger::addTag(const char* FieldName, FieldType type) {
    if (type >= Type_Count)
    {
        return false;
    }
    tagName *newTag = new tagName();
    newTag->tag = FieldName;
    newTag->type = type;
    newTag->columnID = columnIdCount++;
    tags.push_back(newTag);
    entryValues *ev = new entryValues();
    columns.push_back(ev);
    return true;
}

int CSVLogger::findTagID (const char* FieldName) {
    int tagID = -1;
    std::vector<tagName *>::iterator tni, tniEnd;
    tni = tags.begin();
    tniEnd = tags.end();
    for(tni;tni!=tniEnd;tni++){
        if (strcmp( (*tni)->tag, FieldName) == 0){
            tagID = (*tni)->columnID;
        }
    }
    return tagID;
}

FieldType CSVLogger::findTagType (const char* FieldName) {
    FieldType tagType;
    std::vector<tagName *>::iterator tni, tniEnd;
    tni = tags.begin();
    tniEnd = tags.end();
    for(tni;tni!=tniEnd;tni++){
        if (strcmp( (*tni)->tag, FieldName) == 0){
            tagType = (*tni)->type;
        }
    }
    return tagType;
}

void CSVLogger::addFieldValue (const char* FieldName, void * value) {
    int tagID = findTagID(FieldName);
    FieldType tagType = findTagType(FieldName);
    entryValues *ev = columns.at(tagID);
    valueEntry *newEntry = new valueEntry();
    newEntry->fieldName = FieldName;
    newEntry->type = tagType;
    switch (tagType)
    {
        case Char_Type:
            newEntry->charValue = *((unsigned char*)(value));
            break;
        case String_Type:
            newEntry->stringValue =(const char*)(value);
            break;
        case Int_Type:
            newEntry->intValue = *((unsigned int*)(value));
            break;
        case Float_Type:
            newEntry->FloatValue = *((float*)(value));
            break;
        case Double_Type:
            newEntry->DoubleValue = *((double*)(value));
            break;
        default :
            break;
    }

    ev->push_back(newEntry);

    return;
}


void CSVLogger::outputData(FILE *fp,FieldType type, void *value)
{

    switch (type) {
        case Char_Type: 
            fprintf(fp,((char *)value));
            fprintf(fp,", ");
            break;
        case String_Type: 
            fprintf(fp,(const char*)value);
            fprintf(fp,", ");
            break;
        case Int_Type: 
            fprintf(fp,"%d",*((unsigned int *)value));
            fprintf(fp,", ");
            break;
        case Float_Type: 
            fprintf(fp,"%f",*((float*)(value)));
            fprintf(fp,", ");
            break;
        case Double_Type: 
            fprintf(fp,"%lf",*((double*)(value)));
            fprintf(fp,", ");
            break;
        default :
            break;
    }
    return;

}


void CSVLogger::cleanup() {

	if( columns.size() == 0 )
	{
		return;
	}
    entryValues *ev = columns.at(0);
    entryValues *tColumnPtr = 0;
    std::vector<valueEntry *>::iterator vei, veiEnd;
    vei = ev->begin();
    veiEnd = ev->end();
    valueEntry *tve =0;
    int listSize = static_cast<int>(ev->size());
    int listIndex =0;
    for(vei;vei!=veiEnd;vei++){
        for (int cic = 0; cic < columnIdCount; cic++){
            tColumnPtr = columns.at(cic);
            tve = tColumnPtr->at(listIndex);
            delete tve;
            tve = 0;
        }
        listIndex++;

    }


    std::vector<tagName *>::iterator tni, tniEnd;
    tni = tags.begin();
    tniEnd = tags.end();
    listIndex =0;
    for(tni;tni!=tniEnd;tni++){
        delete *tni;
        delete columns.at(listIndex);
        listIndex++;
    }
    tags.clear();
    columns.clear();
}

#pragma warning(disable:4996) //safe func deprecation warning
bool CSVLogger::dumpData(const char* filename ){
    bool retval = true;
    FILE * fp;
    fp = fopen(filename,"wt");

    if (fp == NULL)
    {
        return false;
    }

    std::vector<tagName *>::iterator tni, tniEnd;
    tni = tags.begin();
    tniEnd = tags.end();
    for(tni;tni!=tniEnd;tni++){
        outputData(fp,String_Type,(void *)(*tni)->tag);
    }
    fprintf(fp,"\n");

    std::vector<valueEntry *>::iterator vei, veiEnd;
    entryValues *ev = columns.at(0);
    entryValues *tColumnPtr = 0;
    valueEntry *tve =0;
    /// Assumption is that all columns have same number of data entries
    /// as per JL confirmation of feature.  
    vei = ev->begin();
    veiEnd = ev->end();
	int listSize = static_cast<int>(ev->size());
    int listIndex =0;
    for(vei;vei!=veiEnd;vei++){
        for (int cic = 0; cic < columnIdCount; cic++){
            tColumnPtr = columns.at(cic);
            tve = tColumnPtr->at(listIndex);
            switch ((*tve).type)
            {
                case Char_Type: 
                    outputData(fp, (*tve).type,(void *)&((*tve).charValue));
                    break;
                case String_Type: 
                    outputData(fp, (*tve).type,(void *)(*tve).stringValue);
                    break;
                case Int_Type: 
                    outputData(fp, (*tve).type,(void *)&((*tve).intValue));
                    break;
                case Float_Type: 
                    outputData(fp, (*tve).type,(void *)&((*tve).FloatValue));
                    break;
                case Double_Type: 
                    outputData(fp, (*tve).type,(void *)&((*tve).DoubleValue));
                    break;
                default :
                    break;
            }
        }
        fprintf(fp,"\n");
        listIndex++;
    }

    fclose(fp);
    cleanup();
    return retval;
}
#pragma warning(default:4996)

CSVLogger::CSVLogger()
{
    columnIdCount = 0;
    firstValue = false;
}

CSVLogger::~CSVLogger()
{
	cleanup();
}
