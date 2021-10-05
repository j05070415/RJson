## 背景
目前，常见C++ JSON库有jsoncpp，libjson，rapidjson以及Qt的QJsonDocument，但是或多或少都有些问题，导致得额外使用一些技巧进行处理，不能够按照JSON原生方式进行使用。

* jsoncpp和libjson对于大JSON文件（200M）解析速度非常慢，基本不能使用，优点是易用性好些。
* rapidjson优点非常多，用在生产环境基本没问，例如性能好，自测200M文件解析10s基本搞定（本人笔记本联想T450 2012年的笔记本了），内存占用少，稳定性好，唯一缺点使用复杂，一不小心就会内存泄漏或者崩溃。
* Qt的JSON库也很好用，但是有两个大缺点，超过120M基本不能用，另外对于嵌套内容修改非常麻烦。

基于以上原因，本人基于rapidjson封装了一个非常好用JSON库,它继承了rapidjson优点，能够非常方便对JSON进行增删改查，以及序列化和反序列化操作。

## 简介
JSON库提供对UTF8编码JSON处理，提供RValue和RDocument两个类，定义在RJson命名空间下，主要功能包括：
* RValue
1. 支持多种类型，bool、int32、uint32、int64、uint64、float、double，String、Object、Array；
2. Object类型，提供contains、 keys、remove、值转换和修改操作，另外支持[]、==、!=、=操作符，方便查找和判断；
3. Array类型，提供size、append、remove和last接口，另外支持[]按照下标进行索引；
4. 支持Copy和Move语义。

* RDocument
1. 支持JSON字符串解析和生成；
2. 支持与RValue转换；
3. Object类型，提供contains、 keys、remove、值转换和修改操作，另外支持[]、==、!=、=操作符，方便查找和判断；
4. Array类型，提供size、append、remove和last接口，另外支持[]按照下标进行索引；
5. 支持Copy和Move语义。

## 示例代码
JSON创建
```
RValue o1(alloc);
o1["phone"] = 123455;
o1["name"] = "jone";
o1["addr"] = "xxx@asdfasf";
o1["object"]["name"] = "smith";
o1["object"]["age"] = "13";
o1["array"].append("david");
o1["array"].append(99.1234567);
o1["array"].append(true);
o1["array"].append(-123);
```

JSON序列化和反序列化
```
std::string str = "{\"count\":2,\"names\":[\"zhangsan\",wangwu\"]}";
auto doc1 = RDocument::fromJson(str.c_str(), str.size());
auto text = doc1.toJson();
```

Array类型增删改查
```
std::string str = "{\"count\":2,\"names\":[{\"name\":\"zhangsan\"},{\"name\":\"wangwu\"}]}";
auto doc = RDocument::fromJson(str.c_str(), str.size());
auto names = doc["names"];
printf("size:%d\n", names.size());
for (unsigned int i=0; i<names.size(); ++i) {
    auto name = names[i];
    qDebug() << name["name"].toString().c_str();
}
```

Object类型增删改查
```
RValue o1(alloc);
o1["phone"] = 123455;
o1["name"] = "jone";
o1["name"] = "jone1";
o1.remove("name");
```

空对象创建及修改
```
RValue value(alloc);
value["age"].setObject();
value["times"].setObject();
value["names"].setArray();
value["urls"].setArray();
value["books"].setArray();
```

基本数值类型
```        
int i1 = 0x7FFFFFFF;//2147483647
unsigned int i2 = 0xFFFFFFFF;//4294967295
long long i3 = 0x7FFFFFFFFFFFFFFF;//9223372036854775807
unsigned long long i4 = 0xFFFFFFFFFFFFFFFF;//18446744073709551615
auto txt1 = QString::fromLocal8Bit("hello workd!你好，世界");
std::string s1(txt1.toUtf8().data());

RValue v0, v1(true), v2(110.123), v3(-100), v4(i1),
      v5(i2), v6(i3), v7(i4), v8(s1), v9(s1.c_str()), v10(s1.c_str(), s1.size());
```

### Copyright and Licensing

You can copy and paste the license summary from below.

Licensed under the MIT License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

http://opensource.org/licenses/MIT

Unless required by applicable law or agreed to in writing, software distributed 
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
CONDITIONS OF ANY KIND, either express or implied. See the License for the 
specific language governing permissions and limitations under the License.
