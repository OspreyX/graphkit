/**
* Copyright (C) 2015 GraphKit, Inc. <http://graphkit.io> and Daniel Dahan <daniel@graphkit.io>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program located at the root of the software package
* in a file called LICENSE.  If not, see <http://www.gnu.org/licenses/>.
*
* exports.h
*
* Provides macro methods for integration with node.js and v8 JavaScript Engine.
*/

#ifndef GRAPHKIT_SRC_EXPORTS_H
#define GRAPHKIT_SRC_EXPORTS_H

#include <string>
#include <node.h>
#include <node_object_wrap.h>

// modules and node helpers
#define GK_EXPORT(Args...) void Args(v8::Handle<v8::Object> exports) noexcept

// methods
#define GK_METHOD(Args...) void Args(const v8::FunctionCallbackInfo<v8::Value>& args) noexcept

// initializers and new statements
#define GK_FUNCTION(VALUE) v8::Local<v8::Function>::New(isolate, VALUE)
#define GK_TEMPLATE(Args...) v8::FunctionTemplate::New(isolate, New)
#define GK_CONSTRUCTOR(Args...) v8::Persistent<v8::Function> Args
#define GK_INIT(Args...) void Args(v8::Handle<v8::Object> exports, const char* symbol) noexcept

// index operations
#define GK_INDEX_GETTER(Args...) void Args(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args) noexcept
#define GK_INDEX_SETTER(Args...) void Args(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args) noexcept
#define GK_INDEX_QUERY(Args...) void Args(uint32_t index, const v8::PropertyCallbackInfo<v8::Integer>& args) noexcept
#define GK_INDEX_DELETER(Args...) void Args(uint32_t index, const v8::PropertyCallbackInfo<v8::Boolean>& args) noexcept

// property operations
#define GK_PROPERTY_GETTER(Args...) void Args(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) noexcept
#define GK_PROPERTY_SETTER(Args...) void Args(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args) noexcept
#define GK_PROPERTY_QUERY(Args...) void Args(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Integer>& args) noexcept
#define GK_PROPERTY_DELETER(Args...) void Args(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Boolean>& args) noexcept
#define GK_PROPERTY_ENUMERATOR(Args...) void Args(const v8::PropertyCallbackInfo<v8::Array>& args) noexcept

// scope
#define GK_SCOPE() auto isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate)

// values
#define GK_STRING(VALUE) v8::String::NewFromUtf8(isolate, VALUE)
#define GK_PERSISTENT_STRING(VALUE) v8::Persistent<v8::String>::New(isolate, VALUE)
#define GK_NUMBER(VALUE) v8::Number::New(isolate, VALUE)
#define GK_INTEGER(VALUE) v8::Int32::New(isolate, VALUE)
#define GK_BOOLEAN(VALUE) VALUE ? v8::True(isolate) : v8::False(isolate)
#define GK_UNDEFINED() v8::Undefined(isolate)

// returns
#define GK_RETURN(VALUE) args.GetReturnValue().Set(VALUE); return
#define GK_EXCEPTION(MESSAGE) isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, MESSAGE))); return

#endif