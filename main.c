
//
// Created by chenqy on 10/5/2018.
//

#include "json_utils.h"


char test1[] = "{\"test\":{\"11\":1,\"2 2 \": \n {\"name\":\n\"jake\",\n\"age\":18},"
              "\"33\":\"33 test\"},\"version\":\"1.\t0\",\"aaa\":\"\taaaval\","
              "\"bbb\":true,\"ccc\": 123, \"arr1\": [1,2,3,4,5,6,\r],\"arr2\":"
              "[{\"11\":{\"test\":[11,22,33]}},{\"12\":[\"a123456789\",\"a234567890\","
              "\"a345678901\",\"b123456789\",\"b234567890123456\",\"b345890中文测试\""
              "]},{\"13\":345678900},{\"13\":123123123},{\"13\":90909090}]}";

int testJson(char *test) {
    printf("Hello, World!\n");
    printf("test = %s\n", test);

    jsonObj *rootJson = jsonParse(test);

    printf("aaa = %s\n", getJsonObjString(rootJson, "aaa"));
    printf("bbb = %d\n", getJsonObjBoolean(rootJson, "bbb"));
    printf("ccc = %d\n", getJsonObjInteger(rootJson, "ccc"));
    printf("version = %s\n", getJsonObjString(rootJson, "version"));

    jsonObj *testJson = jsonObject(rootJson, "test");
    printf("test.11 = %d\n", getJsonObjInteger(testJson, "11"));

    jsonObj *test22Json = jsonObject(testJson, "22");
    printf("test.22.name = %s\n", getJsonObjString(test22Json, "name"));
    printf("test.22.age = %d\n", getJsonObjInteger(test22Json, "age"));

    printf("test.33 = %s\n", getJsonObjString(testJson, "33"));

    jsonObj *arr1 = jsonArray(rootJson, "arr1");
    printf("arr1.count=%d\n", arr1->count);
    for(int i=0; i<arr1->count; i++){
        printf("arr1[%d]=%d\n", i, getJsonArrInteger(arr1, i));
    }

    jsonObj *arr2 = jsonArray(rootJson, "arr2");
    printf("arr2.count=%d\n", arr2->count);
    for(int i=0; i<arr2->count; i++){
        jsonObj *subobj= getJsonArrObject(arr2, i);
        jsonObj *sub1obj = jsonObject(subobj, "11");
        if(sub1obj){
            jsonObj *arr3 = jsonArray(sub1obj, "test");
            printf("arr2.11.test.count=%d\n", arr3->count);
            for(int j=0; j<arr3->count; j++){
                printf("arr2.11.test[%d]=%d\n", j, getJsonArrInteger(arr3, j));
            }
        }
        sub1obj = jsonArray(subobj, "12");
        if(sub1obj){
            printf("arr2.12.count=%d\n", sub1obj->count);
            for(int j=0; j<sub1obj->count; j++){
                printf("arr2.12[%d]=%s\n", j, getJsonArrString(sub1obj, j));
            }
        }

        int a = getJsonObjInteger(subobj, "13");
        if(a)
            printf("arr2.13 = %d\n", a);
    }

    return 0;
}

int testParse(jsonObj *obj) {
    float test = getJsonObjFloat(obj,"test");
    uint8_t delay = getJsonObjInteger(obj,"delay");
    printf("testParse===>>>test=%.3f,delay=%d\n", test,delay);
    return 0;
}

static void parse_json_data(jsonObj *root) {
    printf("parse_json_data\n");
    printJsonObj(root);
    int cmd = getJsonObjInteger(root, "cmd");
    switch(cmd){
        case 10:{
            jsonObj *jsonArr = jsonArray(root, "controls");
            if(jsonArr == NULL){
                return;
            }
            int arrSize = jsonArr->count;
            printJsonObj(jsonArr);
            for(int i=0; i<arrSize; i++){
                jsonObj *subobj = getJsonArrObject(jsonArr, i);
                if(subobj != NULL){
                    testParse(subobj);
                }
            }
            break;
        }
        default:break;
    }
}

void testJson1(){
    char buf[]="{\"ver\":\"1.0.1\",\"cmd\":10,\"obj\":1,\"controls\":"
               "{\"tst\":123,\"test\":{},\"qqq\":\"abcdefg123456\"},"
               "\"aa\":123,\"test\":{\"aa\":\"111\"},\"ok\":12}";
    jsonObj *rootJson = jsonParse(buf);
    if(rootJson){
        printJsonObj(rootJson);
        jsonObj *sub1 = jsonObject(rootJson, "controls");
        printJsonObj(rootJson);
        printJsonObj(sub1);
        jsonObj *sub11 = jsonObject(sub1, "test");
        printJsonObj(sub11);
        jsonObj *sub2 = jsonObject(rootJson, "test");
        printJsonObj(sub2);
    }
}

void testJson2(){
    char buf[]="{\"ver\":\"1.0.1\",\"cmd\":10.0,\"obj\":1,\"controls\":"
               "[{\"test\":100,\"delay\":0},{\"test\":201.123,\"delay\":0}]}";
    jsonObj *rootJson = jsonParse(buf);
    if(rootJson){
        parse_json_data(rootJson);
    }
}

void testJsonPack(void){
    jsonPack rootJson;
    createJsonPack(&rootJson);
    if(addJsonStrPair(&rootJson, "aaa", "111")) return;

    jsonPack sub1Obj;
    if(addJsonObject(&rootJson, "key", &sub1Obj)) return;
    if(addJsonBoolPair(&rootJson, "ccc", 1)) return;

    if(addJsonStrPair(&sub1Obj, "111", "222")) return;
    jsonPack sub11Obj;
    if(addJsonObject(&sub1Obj, "add", &sub11Obj)) return;
    if(addJsonIntPair(&sub11Obj, "111", 0xa)) return;

    if(addJsonStrPair(&rootJson, "bbb", "222")) return;

    jsonPack sub1Arr;
    if(addJsonArray(&rootJson,"arrTest", &sub1Arr))  return;
    if(addJsonStrArr(&sub1Arr, "aaa111")) return;

    if(addJsonStrPair(&rootJson, "bbb", "222")) return;
    if(addJsonIntPair(&rootJson, "ccc", -1)) return;

    if(addJsonStrArr(&sub1Arr, "bbb222")) return;

    jsonPack sub2Arr;
    if(addJsonArray(&rootJson,"arr", &sub2Arr))  return;

    jsonPack sub20Arr[5];
    for(int i=0; i<5; i++){
        if(addJsonObjArr(&sub2Arr, &sub20Arr[i])) return;
    }

    if(addJsonStrPair(&sub20Arr[0], "1", "111")) return;
    if(addJsonStrPair(&sub20Arr[0], "2", "222")) return;
    if(addJsonStrPair(&sub20Arr[0], "a", "111")) return;


    if(addJsonStrPair(&sub20Arr[1], "b", "222")) return;

    if(addJsonStrPair(&sub20Arr[2], "c", "333")) return;
    if(addJsonIntPair(&sub20Arr[2], "d", 333)) return;

    if(addJsonStrPair(&sub20Arr[3], "e", "eee")) return;

    if(addJsonIntPair(&sub20Arr[4], "f", 333)) return;


    jsonPack sub3Arr;
    if(addJsonArray(&rootJson,"arrNum", &sub3Arr))  return;

    for(int i=0; i<6; i++){
        if(addJsonIntArr(&sub3Arr, i+10)) return;
    }

    jsonPack sub111Arr;
    if(addJsonArray(&sub11Obj,"arrNumtest", &sub111Arr))  return;
    jsonPack sub111Obj;
    if(addJsonObject(&sub11Obj, "ttt", &sub111Obj)) return;
    if(addJsonStrPair(&sub111Obj, "qqq", "111")) return;
    if(addJsonStrPair(&sub111Obj, "ccc", "222")) return;

    jsonPack sub1111Obj;
    if(addJsonObject(&sub111Obj, "ttt", &sub1111Obj)) return;
    if(addJsonStrPair(&sub1111Obj, "aaa", "000")) return;

    jsonPack sub11111Obj;
    if(addJsonArray(&sub1111Obj,"arr", &sub11111Obj))  return;

    for(int i=0; i<3; i++){
        if(addJsonIntArr(&sub11111Obj, i+30)) return;
    }

    for(int i=0; i<1; i++){
        if(addJsonIntArr(&sub111Arr, i+20)) return;
    }

    printf("json=%s\n",getPackJsonString());

}

int main() {
    testJson(test1);
    testJson1();
    testJson2();

    testJsonPack();
}
