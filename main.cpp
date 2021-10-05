
#include <stdio.h>

#include "RJson.h"

using namespace RJson;
using namespace rapidjson;

void print(const RValue& v) {
    RDocument doc(v);
    printf("%s\n", doc.toJson().c_str());
}

void print(const RDocument& v) {
    printf("%s\n", v.toJson().c_str());
}

template<typename T, typename ...Args>
void print(const T& t, const Args&... args) {
    print(t);
    print(args...);
}

void testRJson() {
    RDocument doc;
    auto alloc = doc.allocator();

    {
        //RValue构造测试
        printf("***************RValue***************\n");
        //数值类型,字符串类型
        int i1 = 0x7FFFFFFF;//2147483647
        unsigned int i2 = 0xFFFFFFFF;//4294967295
        long long i3 = 0x7FFFFFFFFFFFFFFF;//9223372036854775807
        unsigned long long i4 = 0xFFFFFFFFFFFFFFFF;//18446744073709551615
        std::string s1("hello workd!你好，世界");

        RValue v0, v1(true), v2(110.123), v3(-100), v4(i1),
                v5(i2), v6(i3), v7(i4),
                v8(s1), v9(s1.c_str()), v10(s1.c_str(), s1.size());
        print(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10);

        v1 = "changed";
        print(v1);

        //array类型
        RValue a1(alloc);
        a1.append(11);
        a1.append(111.23456);
        a1.append("hello");
        a1[100] = 11;
        print(a1.last());
        print(a1);

        //object类型
//        {"phone":123455,"name":"jone","addr":"xxx@asdfasf",
//        "object":{"name":"smith","age":"13"},
//        "array":["david",99.1234567,true,-123]}
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
        print(o1);

        //拷贝构造和Move语义测试
        printf("copy & move\n");
        o1 = a1;
        print(o1, a1);
        v0 = std::move(a1);
        print(v0, a1);
    }
    {
        //RDocument相关操作
        printf("\n***************RDocument***************\n");
        //array类型
        RDocument a1;
        a1.append(11);
        a1.append(111.23456);
        a1.append("hello");
        a1[100] = 11;
        print(a1.last());
        print(a1);

        //object类型
        RDocument o1;
        o1["phone"] = 123455;
        o1["name"] = "jone";
        o1["addr"] = "xxx@asdfasf";
        print(o1);

        o1.setValue(12345);
        print(o1);

        RDocument d0(true),d1(false),d2(11),d3("hello"),d4(alloc),d5(alloc);
        d4["name"] = "jone";
        d5.append("smith");
        d5.append(12);
        print(d0.value(), d1.value(), d2.value(),
              d3.value(), d4.value(), d5.value(),
              d0,d1,d2,d3,d4,d5);

        //拷贝构造和Move语义测试
        std::string str("北京市天安门人民大会堂1号1室");

        printf("copy & move\n");
        o1 = a1;
        print(o1, a1);
        o1 = RDocument();
        o1["phone"] = "87654321";
        o1["name"] = "smith";
        o1["addr"] = str.c_str();
        a1 = std::move(o1);
        print(o1, a1);
    }

    {
        // 解析字符串
        printf("\nparse JSON string: \n");
        std::string str = "{\"count\":2,\"names\":[\"zhangsan\",\"wangwu\"]}";
        auto doc1 = RDocument::fromJson(str.c_str(), str.size());
        printf("%s\n", doc1.toJson().c_str());
    }

    {
        //重新构造JSON，并转成字符串
        printf("\ncreate JSON, to string: \n");
        RValue j1(alloc);
        j1["name"] = "smith";
        j1["age"] = 11;
        j1["jone"]["name"] = "jone";
        j1["jone"]["age"] = "12";
        printf("%s\n", RDocument(j1).toJson().c_str());
    }

    {
        printf("\nmodify JSON value:\n");
        // 修改JSON对象
        RValue j1(alloc);
        j1["name"] = "smith";
        j1["age"] = 11;
        j1["jone"]["name"] = "jone";
        j1["jone"]["age"] = "12";

        j1["name"] = "smith1";
        j1["age"] = 111;
        j1["jone"]["name"] = "jone1";
        j1["jone"]["age"] = "121";
        printf("%s\n", RDocument(j1).toJson().c_str());
    }

    {
        //读数组
        printf("\nread JSON array:\n");
        std::string str = "{\"count\":2,\"names\":[{\"name\":\"zhangsan\"},{\"name\":\"wangwu\"}]}";

        auto doc = RDocument::fromJson(str.c_str(), str.size());
        auto names = doc["names"];
        print(names);
        printf("size:%d\n", names.size());
        for (unsigned int i=0; i<names.size(); ++i) {
            auto name = names[i];
            print(name);
            printf("%s\n", name["name"].toString().c_str());
        }
    }

    {
        //以Document构造一个json，然后修改它，最后转成字符串
        // 运行输出结果：
        // {"count":3,"names":[{"id":1,"name":"zhangsan"}]}
        // {"count":9,"names":[{"id":1,"name":"lisi"}]}
        printf("\ndocument modify:\n");
        RValue names(alloc);
        names["id"] = 1;
        names["name"] = "zhangsan";
        RValue values(alloc);
        values["count"] = 3;
        values["names"].append(names);
        print(values);

        values["count"] = 9;
        values["names"][0]["name"] = "lisi";
        print(values);
    }

    {
        //构造空对象和数组
        // 运行输出结果：
        // {"age":{},"times":{},"names":[],"urls":[],"books":[]}
        // {"age":6,"times":{},"names":[],"urls":[],"books":[]}
        printf("\nempty value:\n");
        RValue value(alloc);
        value["age"].setObject();
        value["times"].setObject();
        value["names"].setArray();
        value["urls"].setArray();
        value["books"].setArray();
        print(value);
        value["age"] = 6;
        value["times"].setObject();
        value["names"].setArray();
        value["urls"].setArray();
        value["books"].setArray();
        print(value);
    }

    {
        // 删除数组元素
        // 示例运行输出：
        // { "names": [ {"name":"zhangsan","age":100}, {"name":"wangwu","age":90}, {"name":"xiaozhang","age":20} ]}
        // {"names":[{"name":"zhangsan","age":100},{"name":"wangwu","age":90}]}
        printf("\nremove value:\n");
        std::string txt = "{ \"names\": [ {\"name\":\"zhangsan\",\"age\":100}, {\"name\":\"wangwu\",\"age\":90}, {\"name\":\"xiaozhang\",\"age\":20} ]}";
        auto doc = RDocument::fromJson(txt.c_str(), txt.size());
        auto value = doc.value();
        print(doc);
        doc["names"].remove(2);
        print(doc);
        doc["names"][0].remove("age");
        print(doc);

        print(value);
        value["names"].remove(2);
        print(value);
        value["names"][0].remove("age");
        print(value);
    }
}

int main(int, char *[])
{
//    while (true) {
        testRJson();
//    }
    return 0;
}
