/****************************************************************************
**
** Copyright (C) 2019 Ömer Göktaş
** Contact: omergoktas.com
**
** This file is part of the AsyncTest library.
**
** The AsyncTest is free software: you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public License
** version 3 as published by the Free Software Foundation.
**
** The AsyncTest is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with the AsyncTest. If not, see
** <https://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <async.h>
#include <iostream>

struct Foo
{
    void hello() { std::cout << "hello" << std::endl; }
    void hello(const char* msg) { std::cout << msg << std::endl; }
};


int callback(QFutureInterfaceBase* future, std::string e) {
    std::cout << e << std::endl;
    return {};
}

int callback2(QFutureInterfaceBase* future) {
//    std::cout << e << std::endl;
    return {};
}


struct A {
    int a(std::future<int>& future, std::string e) {
        std::cout << e << std::endl;
        return {};
    }

    int b(QFutureInterfaceBase* future, std::string e) {
        std::cout << e << std::endl;
        return {};
    }
};

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    std::function<void(void)> ss;
    A a;
    Async::run([] () { qDebug() << "adasd"; });
    Async::run([] (int) { qDebug() << "adasd 1"; }, 6);
    Async::run([] (int) { qDebug() << "adasd 2"; return 2;}, 6);
    Async::run([] (int e) { qDebug() << "adasd" << e; return 2;}, 3);
    Async::run([] (int e, int k) { qDebug() << "adasd" << e << k; return 2;}, 4, 444);
    Async::run([] (int e, char c) { qDebug() << "adasd" << e << c; return c;}, 5, '@');

    Async::run([] (QFutureInterfaceBase*, auto x, auto y) { return x + y; }, 25, 54);
    Async::run([] (QFutureInterfaceBase* f) { qDebug() << f; return 9; });
    Async::run(std::bind([] () { return "sds"; }));
    Async::run(std::bind([] (QFutureInterfaceBase*) { return "sds"; }, std::placeholders::_1));
    Async::run([] (QFutureInterfaceBase*) { return "sds"; });
    Async::run([] (QFutureInterfaceBase*, char i) { return "sds"; }, 'd');
    Async::run([] (QFutureInterfaceBase*) -> int {});
    Async::run([] (QFutureInterfaceBase*) {});
    Async::run(std::bind<int>([] () -> int {}));
    Async::run(callback2);
    Async::run(&callback2);
    Async::run(callback, std::string("naber 4 yavrum 1"));
    Async::run(&callback, std::string("naber 4 yavrum 1"));
    Async::run(std::function<int(QFutureInterfaceBase*, std::string)>(callback), std::string("naber 4 yavrum 2"));
    Async::run(std::bind(&A::b, &a, std::placeholders::_1, std::placeholders::_2), std::string("naber 4 yavrum 3"));
    Async::run(std::bind(&A::b, &a, std::placeholders::_1, std::string("naber 4 yavrum 4")));
    Async::run(std::bind(&A::b, &a, std::placeholders::_1, std::string("naber 4 yavrum 4.1")), std::string("dd"));

    return app.exec();
}
