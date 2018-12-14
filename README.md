# json_utils  
自己编写一个不用malloc的json解析库，可以解析组装基本的json数据格式。  
$ ./test   
Hello, World!  
test = {"test":{"11":1,"2 2 ":{"name":"jake","age":18},"33":"33 test"},"version":"1. 0","aaa":" aaaval ","bbb":true,"ccc": 123, "arr1": [1,2,3,4,5,6],"arr2":[{"11":{"test":[11,22,33]}},{"12":["a123456789","a234567890","a345678901","b123456789","b234567890123456","b345890中文测试"]},{"13":345678900},{"13":123123123},{"13":90909090}]}  
json={"test":{"11":1,"22":{"name":"jake","age":18},"33":"33test"},"version":"1.0","aaa":"aaaval","bbb":true,"ccc":123,"arr1":[1,2,3,4,5,6,],"arr2":[{"11":{"test":[11,22,33]}},{"12":["a123456789","a234567890","a345678901","b123456789","b234567890123456","b345890中文测试"]},{"13":345678900},{"13":123123123},{"13":90909090}]}  
aaa = aaaval  
bbb = 1  
ccc = 123  
version = 1.0  
test.11 = 1  
test.22.name = jake  
test.22.age = 18  
test.33 = 33test  
arr1.count=6  
arr1[0]=1  
arr1[1]=2  
arr1[2]=3  
arr1[3]=4  
arr1[4]=5  
arr1[5]=6  
arr2.count=5  
arr2.11.test.count=3  
arr2.11.test[0]=11  
arr2.11.test[1]=22   
arr2.11.test[2]=33   
arr2.12.count=6  
arr2.12[0]=a123456789  
arr2.12[1]=a234567890  
arr2.12[2]=a345678901  
arr2.12[3]=b123456789  
arr2.12[4]=b234567890123456  
arr2.12[5]=b345890中文测试  
arr2.13 = 345678900  
arr2.13 = 123123123  
arr2.13 = 90909090  
json={"ver":"1.0.1","cmd":10,"obj":1,"controls":{"tst":123,"test":{},"qqq":"abcdefg123456"},"aa":123,"test":{"aa":"111"},"ok":12}  
--------printJsonObj--------   
ver     ->      "1.0.1"  
cmd     ->      10  
obj     ->      1  
controls        ->      {object}  
aa      ->      123  
test    ->      {object}  
ok      ->      12  
+++++++++++++++++++++++++++  
--------printJsonObj--------  
ver     ->      "1.0.1"  
cmd     ->      10  
obj     ->      1  
controls        ->      {object}  
aa      ->      123  
test    ->      {object}  
ok      ->      12  
+++++++++++++++++++++++++++  
--------printJsonObj--------  
tst     ->      123  
test    ->      {object}  
qqq     ->      "abcdefg123456"  
+++++++++++++++++++++++++++  
--------printJsonObj--------  
------null------  
+++++++++++++++++++++++++++  
--------printJsonObj--------  
aa      ->      "111"  
+++++++++++++++++++++++++++  
json={"ver":"1.0.1","cmd":10.0,"obj":1,"controls":[{"test":100,"delay":0},{"test":201.123,"delay":0}]}  
parse_json_data  
--------printJsonObj--------  
ver     ->      "1.0.1"  
cmd     ->      10.0  
obj     ->      1  
controls        ->      [array]  
+++++++++++++++++++++++++++  
--------printJsonObj--------  
[0]     ->      {object}  
[1]     ->      {object}  
+++++++++++++++++++++++++++  
testParse===>>>test=100.000,delay=0  
testParse===>>>test=201.123,delay=0  
json={"aaa":"111","key":{"111":"222","add":{"111":10,"arrNumtest":[20],"ttt":{"qqq":"111","ccc":"222","ttt":{"aaa":"000","arr":[30,31,32]}}}},"ccc":true,"bbb":"222","arrTest":["aaa111","bbb222"],"bbb":"222","ccc":-1,"arr":[{"1":"111","2":"222","a":"111"},{"b":"222"},{"c":"333","d":333},{"e":"eee"},{"f":333}],"arrNum":[10,11,12,13,14,15]}   
