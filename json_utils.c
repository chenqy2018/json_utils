//
// Created by chenqy on 10/5/2018.
//

#include "json_utils.h"

static jsonObj jsonPairMem[MEM_JSON_SIZE]={0};
static char packJsonMem[PACK_JSON_SIZE]={0};
static int jsonPairIndex = 0;
static int packUseLenght = 0;
static jsonPack_t g_jsonPack;

#define add_index() do{ \
                        jsonPairIndex++; \
                        if(jsonPairIndex >= MEM_JSON_SIZE) return NULL;\
                    }while(0)

/******************** internal calling function *************************/
static VALUE_TYPE_E getType(const char c){
    VALUE_TYPE_E ret;
    switch(c){
        case '\"':
            ret = STRING_TYPE;
            break;
        case 'T':
        case 't':
        case 'F':
        case 'f':
            ret = BOOLEAN_TYPE;
            break;
        case 'n':
        case 'N':
            ret = NULL_TYPE;
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            ret = INTEGER_TYPE;
            break;
        case '[':
            ret = ARRAY_TYPE;
            break;
        case '{':
            ret = OBJECT_TYPE;
            break;
        default:
            ret = INVALID_TYPE;
            break;
    }
    return ret;
}

static void delSpecialChar(char *str){
    char *tmp = str;
    char c = 0;
    while(*tmp != '\0'){
        if((*tmp != ' ')&&(*tmp != '\n')&&(*tmp != '\r')&&(*tmp != '\t')){
            c = *tmp;
            *str = c;
            str++;
        }
        tmp++;
    }
    *str = '\0';
}

static jsonObj *_jsonParsePair(char *str){
    char keyFlag = 0, valFlag = 0;
    int i = 0, idx = 0, leftCnt = 0, rightCnt = 0;
    jsonObj *ret = &jsonPairMem[jsonPairIndex];
    int len = strlen(str);
    if((str[0] != '{')&&(str[len-1] != '}'))
        return NULL;
    if((str[0] == '{')&&(str[1] == '}'))
        return NULL;
    for(i=1; i<len; i++){
        if((str[i] == '\"')&&(keyFlag == 0)){
            jsonPairMem[jsonPairIndex].key = &str[i+1];
            idx = jsonPairIndex;
            keyFlag = 1;
            valFlag = 0;
            leftCnt = 0;
            rightCnt = 0;
            continue;
        }
        if(keyFlag){
            if((str[i] == '\"')&&(str[i+1] == ':')&&(valFlag == 0)){
                VALUE_TYPE_E type = getType(str[i+2]);
                if(type == INVALID_TYPE) return NULL;
                jsonPairMem[idx].valType = type;
                switch(type){
                    case STRING_TYPE:
                        jsonPairMem[idx].value = &str[i+3];
                        break;
                    case BOOLEAN_TYPE:
                    case INTEGER_TYPE:
                    case ARRAY_TYPE:
                    case OBJECT_TYPE:
                    case NULL_TYPE:
                        jsonPairMem[idx].value = &str[i+2];
                        //    printf("###(i=%d)type=%d >>>val=%s\n",idx, type, &str[i+2]);
                        break;
                    default:
                        break;
                }
                valFlag = 1;
                str[i] = '\0';
                continue;
            }
            if(valFlag){
                switch(jsonPairMem[idx].valType){
                    case NULL_TYPE:
                    case BOOLEAN_TYPE:
                    case INTEGER_TYPE:
                        if((str[i] == ',')||(str[i] == '}')){
                            add_index();
                            if(str[i+1] == '\"') {
                                jsonPairMem[idx].nextIdx = jsonPairIndex;
                            }
                            str[i] = '\0';
                            keyFlag = 0;
                        }
                        break;
                    case STRING_TYPE:
                        if(str[i] == '\"'){
                            leftCnt++;
                            if(leftCnt >= 2) {
                                add_index();
                                if ((str[i + 1] == ',') && (str[i + 2] == '\"')) {
                                    jsonPairMem[idx].nextIdx = jsonPairIndex;
                                }
                                str[i] = '\0';
                                keyFlag = 0;
                            }
                        }
                        break;
                    case ARRAY_TYPE:
                        if(str[i] == '[')   leftCnt++;
                        if(str[i] == ']'){
                            rightCnt++;
                            if(leftCnt == rightCnt){
                                add_index();
                                if((str[i+1] == ',')&&(str[i+2] == '\"')){
                                    jsonPairMem[idx].nextIdx = jsonPairIndex;
                                }
                                str[i+1] = '\0';
                                keyFlag = 0;
                            }
                        }
                        break;
                    case OBJECT_TYPE:
                        if(str[i] == '{')   leftCnt++;
                        if(str[i] == '}'){
                            rightCnt++;
                            if(leftCnt == rightCnt){
                                add_index();
                                if((str[i+1] == ',')&&(str[i+2] == '\"')){
                                    jsonPairMem[idx].nextIdx = jsonPairIndex;
                                }
                                str[i+1] = '\0';
                                keyFlag = 0;
                            }
                        }
                        break;
                    default:break;
                }
            }
        }
    }
    return ret;
}

static jsonObj *_jsonParseArr(char *str){
    char valFlag = 0;
    int i = 0, idx = 0, leftCnt = 0, rightCnt = 0, cnt = 0;
    jsonObj *ret = &jsonPairMem[jsonPairIndex];
    int len = strlen(str);
    if((str[0] != '[')&&(str[len-1] != ']'))
        return NULL;
    if((str[0] == '[')&&(str[1] == ']'))
        return NULL;
    VALUE_TYPE_E type = getType(str[1]);
    for(i=1; i<len; i++){
        switch(type) {
            case STRING_TYPE:
                if((str[i] == '\"')&&(valFlag == 0)){
                    jsonPairMem[jsonPairIndex].value = &str[i+1];
                    jsonPairMem[jsonPairIndex].valType = type;
                    idx = jsonPairIndex;
                    valFlag = 1;
                    continue;
                }
                if(valFlag){
                    if(str[i] == '\"'){
                        add_index();
                        if ((str[i+1] == ',') && (str[i+2] == '\"')) {
                            jsonPairMem[idx].nextIdx = jsonPairIndex;
                            cnt++;
                        }
                        str[i] = '\0';
                        valFlag = 0;
                    }
                }
                break;
            case INTEGER_TYPE:
                if(valFlag == 0){
                    jsonPairMem[jsonPairIndex].value = &str[i];
                    jsonPairMem[jsonPairIndex].valType = type;
                    idx = jsonPairIndex;
                    valFlag = 1;
                    continue;
                }
                if(valFlag){
                    if(str[i] == ','){
                        add_index();
                        if (str[i+1] != ']') {
                            jsonPairMem[idx].nextIdx = jsonPairIndex;
                            cnt++;
                        }
                        str[i] = '\0';
                        valFlag = 0;
                    }
                    if(str[i] == ']'){
                        add_index();
                        str[i] = '\0';
                        valFlag = 0;
                    }
                }
                break;
            case OBJECT_TYPE:
                if(valFlag == 0){
                    if (str[i] == '{') {
                        jsonPairMem[jsonPairIndex].value = &str[i];
                        jsonPairMem[jsonPairIndex].valType = type;
                        idx = jsonPairIndex;
                        valFlag = 1;
                    }
                }
                if(valFlag) {
                    if (str[i] == '{') leftCnt++;
                    if (str[i] == '}') {
                        rightCnt++;
                        if (leftCnt == rightCnt) {
                            add_index();
                            if ((str[i + 1] == ',') && (str[i + 2] != ']')) {
                                jsonPairMem[idx].nextIdx = jsonPairIndex;
                                cnt++;
                            }
                            str[i + 1] = '\0';
                            valFlag = 0;
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
    ret->count = cnt+1;
    return ret;
}

/******************** external calling function *************************/
jsonObj *jsonParse(char *str){
    jsonPairIndex = 0;
    memset(jsonPairMem, 0, sizeof(jsonPairMem));
    delSpecialChar(str);
    printf("json=%s\n", str);
    return _jsonParsePair(str);
}

/********************array*************************/
jsonObj *jsonArray(jsonObj *root, const char *key){
    jsonObj *tmp = root;
    while(tmp){
        if(strcmp(tmp->key, key) == 0){
            if(tmp->valType == ARRAY_TYPE)
                return _jsonParseArr(tmp->value);
        }
        if(tmp->nextIdx > 0){
            tmp = &jsonPairMem[tmp->nextIdx];
        }else{
            break;
        }
    }
    return NULL;
}

jsonObj *getJsonArrObject(jsonObj *root, int index){
    if(index >= root->count)  return NULL;
    if(root->valType != OBJECT_TYPE) return NULL;
    jsonObj *tmp = root + index;
    if(tmp == NULL) return NULL;
    return _jsonParsePair(tmp->value);
}

char *getJsonArrString(jsonObj *root, int index){
    if(index >= root->count)  return NULL;
    if(root->valType != STRING_TYPE) return NULL;
    jsonObj *tmp = root + index;
    if(tmp == NULL) return NULL;
    return tmp->value;
}

int getJsonArrInteger(jsonObj *root, int index){
    if(index >= root->count)  return 0;
    if(root->valType != INTEGER_TYPE) return 0;
    jsonObj *tmp = root + index;
    if(tmp == NULL) return 0;
    char *val = tmp->value;
    if(val == NULL) return 0;
    return atoi(val);
}

float getJsonArrFloat(jsonObj *root, int index){
    if(index >= root->count)  return 0;
    if(root->valType != INTEGER_TYPE) return 0;
    jsonObj *tmp = root + index;
    if(tmp == NULL) return 0;
    char *val = tmp->value;
    if(val == NULL) return 0;
    return (float)atof(val);
}

/********************object*************************/
jsonObj *jsonObject(jsonObj *root, const char *key){
    jsonObj *tmp = root;
    while(tmp){
        if(strcmp(tmp->key, key) == 0){
            if(tmp->valType == OBJECT_TYPE)
                return _jsonParsePair(tmp->value);
        }
        if(tmp->nextIdx > 0){
            tmp = &jsonPairMem[tmp->nextIdx];
        }else{
            break;
        }
    }
    return NULL;
}

char *getJsonObjString(jsonObj *obj, const char *key){
    jsonObj *tmp = obj;
    while(tmp){
        if(strcmp(tmp->key, key) == 0){
            if(tmp->valType == STRING_TYPE)
                return tmp->value;
        }
        if(tmp->nextIdx > 0){
            tmp = &jsonPairMem[tmp->nextIdx];
        }else{
            break;
        }
    }
    return NULL;
}

int getJsonObjInteger(jsonObj *obj, const char *key){
    jsonObj *tmp = obj;
    while(tmp){
        if(strcmp(tmp->key, key) == 0){
            if(tmp->valType == INTEGER_TYPE) {
                return atoi(tmp->value);
            }
        }
        if(tmp->nextIdx > 0){
            tmp = &jsonPairMem[tmp->nextIdx];
        }else{
            break;
        }
    }
    return 0;
}

float getJsonObjFloat(jsonObj *obj, const char *key){
    jsonObj *tmp = obj;
    while(tmp){
        if(strcmp(tmp->key, key) == 0){
            if(tmp->valType == INTEGER_TYPE) {
                return (float)atof(tmp->value);
            }
        }
        if(tmp->nextIdx > 0){
            tmp = &jsonPairMem[tmp->nextIdx];
        }else{
            break;
        }
    }
    return 0;
}

int getJsonObjBoolean(jsonObj *obj, const char *key){
    jsonObj *tmp = obj;
    while(tmp){
        if(strcmp(tmp->key, key) == 0){
            if(tmp->valType == BOOLEAN_TYPE) {
                if(!strcmp(tmp->value,"TRUE")||!strcmp(tmp->value,"true")){
                    return 1;
                }else{
                    return 0;
                }
            }
        }
        if(tmp->nextIdx > 0){
            tmp = &jsonPairMem[tmp->nextIdx];
        }else{
            break;
        }
    }
    return 0;
}

/********************test*************************/
void printJsonObj(jsonObj *obj){
    int index = 0;
    jsonObj *tmp = obj;
    printf("--------printJsonObj--------\n");
    if(tmp == NULL) printf("------null------\n");
    while(tmp) {
        if(tmp->key == NULL){
            printf("[%d]\t->\t", index++);
        }else {
            printf("%s\t->\t", tmp->key);
        }
        if (tmp->valType == OBJECT_TYPE){
            printf("{object}\n");
        }
        else if (tmp->valType == ARRAY_TYPE){
            printf("[array]\n");
        }
        else if (tmp->valType == STRING_TYPE){
            printf("\"%s\"\n", tmp->value);
        }
        else{
            printf("%s\n", tmp->value);
        }
        if(tmp->nextIdx > 0){
            tmp = &jsonPairMem[tmp->nextIdx];
        }else{
            break;
        }
    }
    printf("+++++++++++++++++++++++++++\n");
}

/********************jsonPack*************************/
void createJsonPack(jsonPack *root){
    packUseLenght = 0;
    memset(packJsonMem, 0, PACK_JSON_SIZE);
    memset(&g_jsonPack, 0, sizeof(jsonPack_t));
    memset(root, 0, sizeof(jsonPack));
    packJsonMem[packUseLenght++] = '{';
    packJsonMem[packUseLenght++] = '}';
    root->pos = packUseLenght-1;
    g_jsonPack.root = root;
    g_jsonPack.prev = root;
}

int addJsonStrPair(jsonPack *root, const char *key, const char *val){
    int i;
    int cp_size = packUseLenght - root->pos;
    int key_len = strlen(key);
    int val_len = strlen(val);
    int offset = key_len+val_len+5;
    jsonPack *tmpPack = g_jsonPack.root;
    if(packJsonMem[root->pos-1] != '{'){
        offset += 1;
    }
    if((offset+packUseLenght) > PACK_JSON_SIZE){
        return -1;
    }
    while (tmpPack){
        if(tmpPack->pos > root->pos){
            tmpPack->pos += offset;
        }
        tmpPack = tmpPack->next;
    }
    for(i=cp_size; i>0; i--){
        int idx = (i-1)+root->pos;
        packJsonMem[idx+offset] = packJsonMem[idx];
    }
    if(packJsonMem[root->pos-1] != '{'){
        packJsonMem[root->pos++] = ',';
    }
    packJsonMem[root->pos++] = '\"';
    memcpy(&packJsonMem[root->pos], key, key_len);
    root->pos += key_len;
    packJsonMem[root->pos++] = '\"';
    packJsonMem[root->pos++] = ':';
    packJsonMem[root->pos++] = '\"';
    memcpy(&packJsonMem[root->pos], val, val_len);
    root->pos += val_len;
    packJsonMem[root->pos++] = '\"';
    packUseLenght += offset;
    return 0;
}

int addJsonIntPair(jsonPack *root, const char *key, int num){
    int i;
    char tmp[32] = {0};
    int cp_size = packUseLenght - root->pos;
    int key_len = strlen(key);
    sprintf(tmp, "%d", num);
    int val_len = strlen(tmp);
    int offset = key_len+val_len+3;
    jsonPack *tmpPack = g_jsonPack.root;
    if(packJsonMem[root->pos-1] != '{'){
        offset += 1;
    }
    if((offset+packUseLenght) > PACK_JSON_SIZE){
        return -1;
    }
    while (tmpPack){
        if(tmpPack->pos > root->pos){
            tmpPack->pos += offset;
        }
        tmpPack = tmpPack->next;
    }
    for(i=cp_size; i>0; i--){
        int idx = (i-1)+root->pos;
        packJsonMem[idx+offset] = packJsonMem[idx];
    }
    if(packJsonMem[root->pos-1] != '{'){
        packJsonMem[root->pos++] = ',';
    }
    packJsonMem[root->pos++] = '\"';
    memcpy(&packJsonMem[root->pos], key, key_len);
    root->pos += key_len;
    packJsonMem[root->pos++] = '\"';
    packJsonMem[root->pos++] = ':';
    memcpy(&packJsonMem[root->pos], tmp, val_len);
    root->pos += val_len;
    packUseLenght += offset;
    return 0;
}

int addJsonBoolPair(jsonPack *root, const char *key, jsonBool bool){
    int i;
    char *val_t = "true";
    char *val_f = "false";
    int cp_size = packUseLenght - root->pos;
    int key_len = strlen(key);
    int val_len = strlen(val_f);
    if(bool){
        val_len = strlen(val_t);
    }
    int offset = key_len+val_len+3;
    jsonPack *tmpPack = g_jsonPack.root;
    if(packJsonMem[root->pos-1] != '{'){
        offset += 1;
    }
    if((offset+packUseLenght) > PACK_JSON_SIZE){
        return -1;
    }
    while (tmpPack){
        if(tmpPack->pos > root->pos){
            tmpPack->pos += offset;
        }
        tmpPack = tmpPack->next;
    }
    for(i=cp_size; i>0; i--){
        int idx = (i-1)+root->pos;
        packJsonMem[idx+offset] = packJsonMem[idx];
    }
    if(packJsonMem[root->pos-1] != '{'){
        packJsonMem[root->pos++] = ',';
    }
    packJsonMem[root->pos++] = '\"';
    memcpy(&packJsonMem[root->pos], key, key_len);
    root->pos += key_len;
    packJsonMem[root->pos++] = '\"';
    packJsonMem[root->pos++] = ':';
    if(bool) {
        memcpy(&packJsonMem[root->pos], val_t, val_len);
    }else{
        memcpy(&packJsonMem[root->pos], val_f, val_len);
    }
    root->pos += val_len;
    packUseLenght += offset;
    return 0;
}

int addJsonObject(jsonPack *root, const char *key, jsonPack *obj){
    int i;
    int cp_size = packUseLenght - root->pos;
    int key_len = strlen(key);
    int offset = key_len+5;
    jsonPack *tmpPack = g_jsonPack.root;
    if(packJsonMem[root->pos-1] != '{'){
        offset += 1;
    }
    if((offset+packUseLenght) > PACK_JSON_SIZE){
        return -1;
    }
    while (tmpPack){
        if(tmpPack->pos > root->pos){
            tmpPack->pos += offset;
        }
        tmpPack = tmpPack->next;
    }
    for(i=cp_size; i>0; i--){
        int idx = (i-1)+root->pos;
        packJsonMem[idx+offset] = packJsonMem[idx];
    }
    if(packJsonMem[root->pos-1] != '{'){
        packJsonMem[root->pos++] = ',';
    }
    packJsonMem[root->pos++] = '\"';
    memcpy(&packJsonMem[root->pos], key, key_len);
    root->pos += key_len;
    packJsonMem[root->pos++] = '\"';
    packJsonMem[root->pos++] = ':';
    packJsonMem[root->pos++] = '{';
    packJsonMem[root->pos++] = '}';
    memset(obj, 0, sizeof(jsonPack));
    obj->pos = root->pos-1;
    packUseLenght += offset;
    g_jsonPack.prev->next = obj;
    g_jsonPack.prev = obj;
    return 0;
}

int addJsonArray(jsonPack *root, const char *key, jsonPack *obj){
    int i;
    int cp_size = packUseLenght - root->pos;
    int key_len = strlen(key);
    int offset = key_len+5;
    jsonPack *tmpPack = g_jsonPack.root;
    if(packJsonMem[root->pos-1] != '{'){
        offset += 1;
    }
    if((offset+packUseLenght) > PACK_JSON_SIZE){
        return -1;
    }
    while (tmpPack){
        if(tmpPack->pos > root->pos){
            tmpPack->pos += offset;
        }
        tmpPack = tmpPack->next;
    }
    for(i=cp_size; i>0; i--){
        int idx = (i-1)+root->pos;
        packJsonMem[idx+offset] = packJsonMem[idx];
    }
    if(packJsonMem[root->pos-1] != '{'){
        packJsonMem[root->pos++] = ',';
    }
    packJsonMem[root->pos++] = '\"';
    memcpy(&packJsonMem[root->pos], key, key_len);
    root->pos += key_len;
    packJsonMem[root->pos++] = '\"';
    packJsonMem[root->pos++] = ':';
    packJsonMem[root->pos++] = '[';
    packJsonMem[root->pos++] = ']';
    memset(obj, 0, sizeof(jsonPack));
    obj->pos = root->pos-1;
    packUseLenght += offset;
    g_jsonPack.prev->next = obj;
    g_jsonPack.prev = obj;
    return 0;
}

int addJsonObjArr(jsonPack *root, jsonPack *obj){
    int i;
    int cp_size = packUseLenght - root->pos;
    int offset = 2;
    jsonPack *tmpPack = g_jsonPack.root;
    if(packJsonMem[root->pos-1] != '['){
        offset += 1;
    }
    if((offset+packUseLenght) > PACK_JSON_SIZE){
        return -1;
    }
    while (tmpPack){
        if(tmpPack->pos > root->pos){
            tmpPack->pos += offset;
        }
        tmpPack = tmpPack->next;
    }
    for(i=cp_size; i>0; i--){
        int idx = (i-1)+root->pos;
        packJsonMem[idx+offset] = packJsonMem[idx];
    }
    if(packJsonMem[root->pos-1] != '['){
        packJsonMem[root->pos++] = ',';
    }
    packJsonMem[root->pos++] = '{';
    packJsonMem[root->pos++] = '}';
    memset(obj, 0, sizeof(jsonPack));
    obj->pos = root->pos-1;
    packUseLenght += offset;
    g_jsonPack.prev->next = obj;
    g_jsonPack.prev = obj;
    return 0;
}

int addJsonStrArr(jsonPack *root, const char *val){
    int i;
    int cp_size = packUseLenght - root->pos;
    int val_len = strlen(val);
    int offset = val_len+2;
    jsonPack *tmpPack = g_jsonPack.root;
    if(packJsonMem[root->pos-1] != '['){
        offset += 1;
    }
    if((offset+packUseLenght) > PACK_JSON_SIZE){
        return -1;
    }
    while (tmpPack){
        if(tmpPack->pos > root->pos){
            tmpPack->pos += offset;
        }
        tmpPack = tmpPack->next;
    }
    for(i=cp_size; i>0; i--){
        int idx = (i-1)+root->pos;
        packJsonMem[idx+offset] = packJsonMem[idx];
    }
    if(packJsonMem[root->pos-1] != '['){
        packJsonMem[root->pos++] = ',';
    }
    packJsonMem[root->pos++] = '\"';
    memcpy(&packJsonMem[root->pos], val, val_len);
    root->pos += val_len;
    packJsonMem[root->pos++] = '\"';
    packUseLenght += offset;
    return 0;
}

int addJsonIntArr(jsonPack *root, int num){
    int i;
    char val[32]={0};
    int cp_size = packUseLenght - root->pos;
    sprintf(val, "%d", num);
    int val_len = strlen(val);
    int offset = val_len;
    jsonPack *tmpPack = g_jsonPack.root;
    if(packJsonMem[root->pos-1] != '['){
        offset += 1;
    }
    if((offset+packUseLenght) > PACK_JSON_SIZE){
        return -1;
    }
    while (tmpPack){
        if(tmpPack->pos > root->pos){
            tmpPack->pos += offset;
        }
        tmpPack = tmpPack->next;
    }
    for(i=cp_size; i>0; i--){
        int idx = (i-1)+root->pos;
        packJsonMem[idx+offset] = packJsonMem[idx];
    }
    if(packJsonMem[root->pos-1] != '['){
        packJsonMem[root->pos++] = ',';
    }
    memcpy(&packJsonMem[root->pos], val, val_len);
    root->pos += val_len;
    packUseLenght += offset;
    return 0;
}

char *getPackJsonString(void){
    return packJsonMem;
}
