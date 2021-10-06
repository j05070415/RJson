/****************************************************************************
** Copyright (C) 2021. All rights reserved.
**
** Licensed under the MIT License (the "License"); you may not use this file except
** in compliance with the License. You may obtain a copy of the License at
**
** http://opensource.org/licenses/MIT
**
** Unless required by applicable law or agreed to in writing, software distributed
** under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
** CONDITIONS OF ANY KIND, either express or implied. See the License for the
** specific language governing permissions and limitations under the License.
****************************************************************************/
#ifndef __RJson_H__
#define __RJson_H__

#include <string>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace RJson {
/**
 * @brief RValue类代表JSON中值类型，支持多种类型数据,例如数值类型、对象类型和数组类型。
 * @code 典型用法
 *      RValue o1(alloc);
 *      o1["phone"] = 123455;
 *      o1["name"] = "jone";
 *      o1["addr"] = "xxx@asdfasf";
 *      o1["object"]["name"] = "smith";
 *      o1["object"]["age"] = "13";
 *      o1["array"].append("david");
 *      o1["array"].append(99.1234567);
 *      o1["array"].append(true);
 *      o1["array"].append(-123);
 *      print(o1)
 * 输出：
 *      {"phone":123455,"name":"jone","addr":"xxx@asdfasf",
 *      "object":{"name":"smith","age":"13"},
 *      "array":["david",99.1234567,true,-123]}
 */
template<typename Allocator = rapidjson::MemoryPoolAllocator<>>
class GenericRValue {
public:
    //构造函数
    GenericRValue(Allocator* alloc = nullptr)
        : _value(new rapidjson::Value), _allocator(alloc) { _value->SetNull(); }
    GenericRValue(bool b, Allocator* alloc = nullptr)
        : _value(new rapidjson::Value), _allocator(alloc) { setValue(b); }
    GenericRValue(double d, Allocator* alloc = nullptr)
        : _value(new rapidjson::Value), _allocator(alloc) { setValue(d); }
    GenericRValue(int n, Allocator* alloc = nullptr)
        : _value(new rapidjson::Value), _allocator(alloc) { setValue(n); }
    GenericRValue(unsigned int n, Allocator* alloc = nullptr)
        : _value(new rapidjson::Value), _allocator(alloc) { setValue(n); }
    GenericRValue(long long n, Allocator* alloc = nullptr)
        : _value(new rapidjson::Value), _allocator(alloc) { setValue(n); }
    GenericRValue(unsigned long long n, Allocator* alloc = nullptr)
        : _value(new rapidjson::Value), _allocator(alloc) { setValue(n); }
    GenericRValue(const std::string &s, Allocator* alloc = nullptr)
        : _value(new rapidjson::Value), _allocator(alloc) { setValue(s); }
    GenericRValue(const char *s, Allocator* alloc = nullptr)
        : _value(new rapidjson::Value), _allocator(alloc) { setValue(s); }
    GenericRValue(const char *s, int size, Allocator* alloc = nullptr)
        : _value(new rapidjson::Value), _allocator(alloc) { setValue(s, size); }

    //拷贝构造实现
    GenericRValue(const GenericRValue &other)
        : _value(new rapidjson::Value){
        if (_allocator != nullptr)
            _value->CopyFrom(*other._value, *_allocator);
        _own = true;
        _allocator = other._allocator;
    }

    GenericRValue(GenericRValue &&other)
        : _value(other._value)
        , _own(other._own)
        , _allocator(other._allocator) {
        other._value = nullptr;
        other._own = false;
    }
    ~GenericRValue() {
        if (_own) { delete _value; }
    }

    //判断值类型
    bool isArray() const { return _value->IsArray(); }
    bool isBool() const { return _value->IsBool(); }
    bool isDouble() const { return _value->IsDouble(); }
    bool isNull() const { return _value->IsNull(); }
    bool isObject() const { return _value->IsObject(); }
    bool isString() const { return _value->IsString(); }

    //值转换
    bool toBool(bool defaultValue = false) const {
        if (!_value->IsBool()) return defaultValue;
        return _value->GetBool();
    }
    double toDouble(double defaultValue = 0) const {
        if (!_value->IsDouble()) return defaultValue;
        return _value->GetDouble();
    }
    int toInt(int defaultValue = 0) const {
        if (!_value->IsInt()) return defaultValue;
        return _value->GetInt();
    }
    unsigned int toUInt(unsigned int defaultValue = 0) const {
        if (!_value->IsUint()) return defaultValue;
        return _value->GetUint();
    }
    long long toLonglong(int defaultValue = 0) const {
        if (!_value->IsInt64()) return defaultValue;
        return _value->GetInt64();
    }
    unsigned long long toULonglong(int defaultValue = 0) const {
        if (!_value->IsUint64()) return defaultValue;
        return _value->GetUint64();
    }
    std::string toString(const std::string &defaultValue = "") const {
        if (!_value->IsString()) return defaultValue;
        return _value->GetString();
    }

    //修改值
    void setValue(bool b) { _value->SetBool(b); }
    void setValue(double d) { _value->SetDouble(d); }
    void setValue(int n) { _value->SetInt(n); }
    void setValue(unsigned int n) { _value->SetUint(n); }
    void setValue(long long n) { _value->SetInt64(n); }
    void setValue(unsigned long long n) { _value->SetUint64(n); }
    void setValue(const std::string &s) { _value->SetString(s.c_str(), static_cast<rapidjson::SizeType>(s.size())); }
    void setValue(const char *s) { _value->SetString(s, static_cast<rapidjson::SizeType>(strlen(s))); }
    void setValue(const char *s, int size) { _value->SetString(s, size); }
    void setValue(const GenericRValue &other) { _value->CopyFrom(*other._value, *_allocator); }
    void reset() { _value->SetNull(); }

    /// 初始化空对象
    void setObject() { _value->SetObject(); }
    /// 初始化空数组
    void setArray() { _value->SetArray(); }

    //允许外部修改分配器,有可能导致崩溃，在不了解分配器原理情况下，不建议使用
    void setAllocator(Allocator* alloc) { _allocator = alloc; }
    Allocator* allocator() const { return _allocator; }

    //操作符相关函数
    bool operator==(const GenericRValue &other) const {
        return *_value == *other._value;
    }
    bool operator!=(const GenericRValue &other) const{
        return *_value != *other._value;
    }
    GenericRValue& operator=(const GenericRValue &other) {
        if (this != &other && _allocator != nullptr) {
            _value->CopyFrom(*other._value, *_allocator);
        }

        return *this;
    }
    GenericRValue& operator=(GenericRValue &&other) {
        if (this != &other) {
            if (_own && _value != nullptr)
                delete _value;
            _value = other._value;
            _own = other._own;
            other._value = nullptr;
            other._own = false;
        }

        return *this;
    }

    //对象类型操作函数
    /// 判断是否存在key键
    bool contains(const std::string &key) {
        if (!_value->IsObject()) return false;

        return _value->HasMember(key.c_str());
    }

    void remove(const std::string &key) {
        if (!_value->IsObject()) return;

        _value->RemoveMember(key.c_str());
    }

    /// 按照key键索引其对应的值，只对Object类型有效
    GenericRValue operator[](const std::string &key) const {
        if (_allocator == nullptr) {
            printf("Alloctor is null, RValue construct with no alloctor\n");
            return {};
        }
        if (_value->IsNull()) {
            _value->SetObject();
        }
        if (!_value->IsObject()) {
            printf("value is not an object!\n");
            return {};
        }
        if (!_value->HasMember(key.c_str())) {
            //必须拷贝key内容
            rapidjson::Value lvalue(key.c_str(), key.size(), *_allocator);
            _value->AddMember(lvalue, {}, *_allocator);
        }

        auto& v = (*_value)[key.c_str()];
        return GenericRValue(&v, _allocator);
    }

    std::vector<std::string> keys() const {
        if (!_value->IsObject()) {
            printf("keys value is not an object!\n");
            return {};
        }

        std::vector<std::string> result;
        auto iter = _value->MemberBegin();
        while (iter != _value->MemberEnd()) {
            result.push_back(iter->name.GetString());
            ++iter;
        }
        return result;
    }

    //数组类型操作函数
    GenericRValue operator[](unsigned int i) const {
        if (!_value->IsArray()) {
            printf("RValue is not an array\n");
            return {};
        }

        auto count = _value->Size();
        if (i >= count) {
            printf("RValue index out of range\n");
            return {};
        }
        auto& value = _value->GetArray()[i];
        return GenericRValue(&value, _allocator);
    }

    unsigned int size() const {
        if (!_value->IsArray()) {
            printf("RValue is not an array, no size!\n");
            return 0;
        }
        return _value->Size();
    }

    void append(const GenericRValue& value) {
        if (_value->IsNull())
            _value->SetArray();
        if (_allocator == nullptr) {
            printf("RValue has not allocator, can not append!\n");
            return;
        }
        if (!_value->IsArray()) {
            printf("RValue is not an array, can not append!\n");
            return;
        }

        _value->PushBack(*value._value, *_allocator);
    }

    GenericRValue last() {
        if (!_value->IsArray()) {
            printf("RValue is not an array, can not last!\n");
            return {};
        }

        auto iter = _value->End()-1;
        return GenericRValue(&(*iter), _allocator);
    }

    void remove(int i, int n = 1) {
        if (!_value->IsArray()) {
            printf("RValue is not an array, can not remove!\n");
            return;
        }

        if (i<0 || i+n>_value->Size()) {
            printf("RValue remove index out of range,start:%d,end:%d\n", i, i+n);
            return;
        }

        auto begin = _value->Begin()+i;
        auto end = begin+n;
        _value->Erase(begin, end);
    }

    void clear() {
        _value->Clear();
    }

private:
    GenericRValue(rapidjson::Value* other, Allocator* alloc)
        : _value(other), _allocator(alloc), _own(false) {
    }

private:
    friend class RDocument;
    Allocator* _allocator = nullptr;
    rapidjson::Value* _value = nullptr;

    /// True时,析构函数释放_value指向对象;否则,不释放_value指向对象
    bool _own = true;
};

using RValue = GenericRValue<>;

/**
 * @brief RDocument类用来提供JSON文档读写能力。
 * 支持UTF8编码的序列化和反序列化功能，例如从JSON对象转换成字符串，toJson(); 从字符串转换成JSON对象，fromJson()。
 * 提供JSON值互相转换功能，方便值操作，例如value()。
 * 提供基于文档访问值功能，例如operator []。
 * 提供对象类型增删改查功能
 * 提供数组类型增删改查功能
 * @code 典型用法
 *      std::string txt = "{ \"names\": [ {\"name\":\"zhangsan\",\"age\":100}, {\"name\":\"wangwu\",\"age\":90}, {\"name\":\"xiaozhang\",\"age\":20} ]}";
 *      auto doc = RDocument::fromJson(txt.c_str(), txt.size());
 *      auto value = doc.value();
 *      doc["names"].remove(2);
 *      doc["names"][0].remove("age");
 *      print(doc);
 *
 *      value["names"].remove(2);
 *      value["names"][0].remove("age");
 *      print(value);
 * 输出:
 *      {"names":[{"name":"zhangsan"},{"name":"wangwu","age":90}]}
 *      {"names":[{"name":"zhangsan"},{"name":"wangwu","age":90}]}
 */
class RDocument {
public:
    RDocument() {}
    /**
     * @brief RDocument将value对象转成documnet对象。
     * @code
     *  RDocument doc;
     *  auto alloc = doc.allocator();
     *  RValue j1(alloc);
     *  j1["name"] = "smith";
     *  j1["age"] = 11;
     *  j1["jone"]["name"] = "jone";
     *  j1["jone"]["age"] = "12";
     *  auto text = RDocument(j1).toJson();
     * @param object为JSON值对象
     */
    RDocument(const RValue &object) {
        _doc.CopyFrom(*(object._value), _doc.GetAllocator());
    }

    RDocument(const RDocument &other) {
        _doc.CopyFrom(other._doc, _doc.GetAllocator());
    }

    RDocument(RDocument &&other) : _doc(std::move(other._doc)) {}

    ~RDocument() {}

    bool isObject() const { return _doc.IsObject(); }
    bool isArray() const { return _doc.IsArray(); }
    bool isNumber() const { return _doc.IsNumber(); }
    bool isString() const { return _doc.IsString(); }
    bool isBool() const { return _doc.IsBool(); }
    bool isNull() const { return _doc.IsNull(); }

    RValue value() {
        RValue value(&_doc.GetAllocator());
        auto type = _doc.GetType();
        switch (type) {
        case rapidjson::kFalseType :
        case rapidjson::kTrueType  :
            value._value->SetBool(_doc.GetBool());
            break;
        case rapidjson::kObjectType: {
            auto iter = _doc.MemberBegin();
            while (iter != _doc.MemberEnd()) {
                value[iter->name.GetString()]._value->CopyFrom(iter->value, _doc.GetAllocator());
                ++iter;
            }
            break;
        } case rapidjson::kArrayType : {
            auto iter = _doc.Begin();
            while (iter != _doc.End()) {
                rapidjson::Value tmp;
                tmp.CopyFrom(*iter, _doc.GetAllocator());
                value.append(RValue(&tmp, &_doc.GetAllocator()));
                ++iter;
            }
            break;
        } case rapidjson::kStringType:
            value._value->SetString(_doc.GetString(),
                                    _doc.GetStringLength(),
                                    _doc.GetAllocator());
            break;
        case rapidjson::kNumberType:
            if (_doc.IsInt()) { value._value->SetInt(_doc.GetInt()); }
            else if (_doc.IsUint()) { value._value->SetUint(_doc.GetUint()); }
            else if (_doc.IsInt64()) { value._value->SetInt64(_doc.GetInt64()); }
            else if (_doc.IsUint64()) { value._value->SetUint64(_doc.GetUint64()); }
            else if (_doc.IsFloat()) { value._value->SetFloat(_doc.GetFloat()); }
            else if (_doc.IsDouble()) { value._value->SetDouble(_doc.GetDouble()); }
            break;
        default:
            printf("unknow type:%d\n", type);
            break;
        }
        return value;
    }

    void setValue(const RValue& v) {
        _doc.CopyFrom(*v._value, _doc.GetAllocator());
    }

    bool operator==(const RDocument &other) const { return _doc == other._doc; }
    bool operator!=(const RDocument &other) const { return _doc != other._doc; }
    RDocument& operator=(const RDocument &other) {
        if (this != &other) {
            _doc.CopyFrom(other._doc, _doc.GetAllocator());
        }

        return *this;
    }

    RDocument& operator=(RDocument &&other) {
        if (this != &other) {
            _doc = std::move(other._doc);
        }

        return *this;
    }

    bool contains(const std::string &key) {
        if (!_doc.IsObject()) return false;

        return _doc.HasMember(key.c_str());
    }

    void remove(const std::string &key) {
        if (!_doc.IsObject()) return;

        _doc.RemoveMember(key.c_str());
    }

    std::vector<std::string> keys() const {
        if (!_doc.IsObject()) {
            printf("document keys() value is not an object!\n");
            return {};
        }

        std::vector<std::string> result;
        auto iter = _doc.MemberBegin();
        while (iter != _doc.MemberEnd()) {
            result.push_back(iter->name.GetString());
            ++iter;
        }
        return result;
    }

    RValue operator[](const std::string &key) const {
        if (_doc.IsNull())
            _doc.SetObject();
        if (!_doc.IsObject()) {
            printf("RDocument is not an object\n");
            return RValue(&_doc.GetAllocator());
        }

        if (!_doc.HasMember(key.c_str())) {
            //必须拷贝key内容
            rapidjson::Value lvalue(key.c_str(), key.size(), _doc.GetAllocator());
            _doc.AddMember(lvalue, {}, _doc.GetAllocator());
        }

        auto& value = _doc[key.c_str()];
        return RValue(&value, &_doc.GetAllocator());
    }

    RValue operator[](unsigned int i) const {
        if (!_doc.IsArray()) {
            printf("RDocument is not an array\n");
            return {};
        }

        auto count = _doc.Size();
        if (i<0 || i>=count) {
            printf("RDocument index out of range\n");
            return {};
        }
        auto& value = _doc[i];
        return RValue(&value, &_doc.GetAllocator());
    }

    int size() const {
        if (!_doc.IsArray()) {
            printf("RDocument is not an array!\n");
            return 0;
        }
        return _doc.Size();
    }

    void append(const RValue& value) {
        if (_doc.IsNull())
            _doc.SetArray();
        if (!_doc.IsArray()) {
            printf("RDocument is not an array, can not append!\n");
            return;
        }

        _doc.PushBack(*value._value, _doc.GetAllocator());
    }

    RValue last() {
        if (!_doc.IsArray()) {
            printf("RDocument is not an array, can not last!\n");
            return {};
        }

        auto iter = _doc.End()-1;
        return RValue(&(*iter), &_doc.GetAllocator());
    }

    void remove(unsigned int i, unsigned int n) {
        if (!_doc.IsArray()) {
            printf("RDocument is not an array, can not remove!\n");
            return;
        }

        if (i+n>_doc.Size()) {
            printf("remove index out of range:start:%u,end:%u\n", i, i+n);
            return;
        }

        auto begin = _doc.Begin()+i;
        auto end = begin+n;
        _doc.Erase(begin, end);
    }

    void clear() {
        _doc.Clear();
    }

    std::string toJson() const {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        _doc.Accept(writer);
        return buffer.GetString();
    }

    /// 由于Rapidjson使用要求,RDocument类提供分配器获取接口,保证内存高效分配及统一释放
    template<typename T = rapidjson::MemoryPoolAllocator<>>
    T* allocator() {
        return &_doc.GetAllocator();
    }

public:
    static RDocument fromJson(const char* data, size_t size) {
        RDocument d;
        d._doc.Parse(data, size);
        return d;
    }

private:
    mutable rapidjson::Document _doc;
};
}

#endif// __RJson_H__
