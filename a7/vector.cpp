#include <iostream>
#include <cassert>
#include <cmath>
#include <stdlib.h>     
#include <time.h>       

#include <vector>

using namespace std;

class Vector3d {
public:
    Vector3d() : x_(0), y_(0.0), z_(0) { }
    Vector3d(double x, double y, double z) : x_(x), y_(y), z_(z) { }
    Vector3d(const Vector3d &vector);
    Vector3d& operator= (const Vector3d &vector);

    double& operator[] (const int index);

    Vector3d operator-();
    bool operator!();

    friend bool operator==(Vector3d v1, Vector3d v2);
    friend bool operator!=(Vector3d v1, Vector3d v2);

    friend Vector3d operator+(const Vector3d &v1, const Vector3d &v2);
    friend Vector3d operator-(const Vector3d &v1, const Vector3d &v2);
    friend Vector3d operator*(const Vector3d &v, double scalar);
    friend Vector3d operator*(double scalar, const Vector3d &v);
    friend Vector3d operator/(const Vector3d &v, double scalar);

    Vector3d operator+=(const Vector3d &v);
    Vector3d operator-=(const Vector3d &v);
    Vector3d operator*=(double scalar);

    friend ostream& operator<< (ostream &out, Vector3d &vector);
    friend istream& operator>> (istream &in, Vector3d &vector);

    friend Vector3d cross(const Vector3d &v1, const Vector3d &v2);
    friend double dot(const Vector3d &v1, const Vector3d &v2);
    friend double operator*(const Vector3d &v1, const Vector3d &v2);
    void normalize();
    double length() const;

    double getX() const { return x_; }
    void setX(double x) { x_ = x; }
    double getY() const { return y_; }
    void setY(double y) { y_ = y; }
    double getZ_() const { return z_; }
    void setZ_(double z) { z_ = z; }

private:
    double x_;
    double y_;
    double z_;
};

double &Vector3d::operator[](const int nIndex) {
    assert(nIndex >= 0 && nIndex <= 3);
    switch (nIndex) {
        case 0: return x_;
        case 1: return y_;
        case 2: return z_;
    }
}

Vector3d::Vector3d(const Vector3d &vector) {
    x_ = vector.x_;
    y_ = vector.y_;
    z_ = vector.z_;
}

Vector3d &Vector3d::operator=(const Vector3d &vector) {
    // check for self-assignment
    if (this == &vector) {
        return *this;
    }
    x_ = vector.x_;
    y_ = vector.y_;
    z_ = vector.z_;
    return *this;
}

Vector3d Vector3d::operator+=(const Vector3d &vector) {
    x_ += vector.x_;
    y_ += vector.y_;
    z_ += vector.z_;
    return *this;
}

Vector3d Vector3d::operator-=(const Vector3d &vector) {
    x_ -= vector.x_;
    y_ -= vector.y_;
    z_ -= vector.z_;
    return *this;
}

Vector3d Vector3d::operator*=(double scalar) {
    x_ *= scalar;
    y_ *= scalar;
    z_ *= scalar;
    return *this;
}

ostream &operator<<(ostream &out, Vector3d &vector) {
    out << "(" << vector.x_ << ", " <<
    vector.y_ << ", " <<
    vector.z_ << ")";
    return out;
}

istream& operator>>(istream &in, Vector3d &vector) {
    in >> vector.x_;
    in >> vector.y_;
    in >> vector.z_;
    return in;
}

Vector3d Vector3d::operator-() {
    return Vector3d(-x_, -y_, -z_);
}

bool Vector3d::operator!() {
    return (x_ == 0.0
            && y_ == 0.0
            && z_ == 0.0);
}

bool operator==(Vector3d v1, Vector3d v2) {
    return (v1.x_ == v2.x_
            && v1.y_ == v2.y_
            && v1.z_ == v2.z_);
}

bool operator!=(Vector3d v1, Vector3d v2) {
    return !(v1 == v2);
}

Vector3d operator+(const Vector3d &v1, const Vector3d &v2) {
    return Vector3d{v1.x_+v2.x_, v1.y_+v2.y_, v1.z_+v2.z_};
}

Vector3d operator-(const Vector3d &v1, const Vector3d &v2) {
    return Vector3d{v1.x_-v2.x_, v1.y_-v2.y_, v1.z_-v2.z_};
}

Vector3d operator*(Vector3d const &v, double scalar) {
    return Vector3d{v.x_*scalar, v.y_*scalar, v.z_*scalar};
}

Vector3d operator*(double scalar, Vector3d const &v) {
    return v*scalar;
}

Vector3d operator/(const Vector3d &v, double scalar) {
    assert(scalar);
    return Vector3d{v.x_/scalar, v.y_/scalar, v.z_/scalar};
}

double dot(const Vector3d &v1, const Vector3d &v2) {
    return v1.x_*v2.x_ + v1.y_*v2.y_ + v1.z_*v2.z_;
}

double operator*(const Vector3d &v1, const Vector3d &v2) {
    return dot(v1, v2);
}

Vector3d cross(const Vector3d &v1, const Vector3d &v2) {
    return Vector3d{
            v1.y_*v2.z_ - v1.z_*v2.y_,
            v1.z_*v2.x_ - v1.x_*v2.z_,
            v1.x_*v2.y_ - v1.y_*v2.x_};
}

double Vector3d::length() const {
    return std::sqrt(*this * *this);
}

void Vector3d::normalize() {
    double len = length();
    x_ = x_ / len;
    y_ = y_ / len;
    z_ = z_ / len;
}

void test() {
    // test default constructor
    Vector3d v1;  
    std::cout << "test: default ctor" << std::endl;
    std::cout << "(0.0,0.0,0.0):\t" << v1 << std::endl; 
    assert(v1[0]==0.0 && v1[1]==0.0 && v1[2]==0.0); 

    // test init-list
    std::cout << "\ntest: init-list ctor" << std::endl;
    Vector3d v2 {2.0, 3.0, 4.0};
    std::cout << "(2.0,3.0,4.0):\t" << v2 << std::endl; 
    assert(v2[0]==2.0 && v2[1]==3.0 && v2[2]==4.0); 

    // test assignment
    std::cout << "\ntest: assignment" << std::endl;
    Vector3d v3; 
    v3 = v2; 
    std::cout << "(2.0,3.0,4.0):\t" << v3 << std::endl; 
    assert(v3[0]==2.0 && v3[1]==3.0 && v3[2]==4.0); 

    // test init-list for containers
    std::cout << "\ntest: init list for container" << std::endl;
    std::cout << "the x, y, and z axis:" << std::endl;
    std::vector<Vector3d> vc {{1.0,0.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0}};
    for(auto v : vc) std::cout << v << "\t"; std::cout << std::endl;
    assert(vc[0][0]==1.0 && vc[0][1]==0.0 && vc[0][2]==0.0); 
    assert(vc[1][0]==0.0 && vc[1][1]==1.0 && vc[1][2]==0.0); 
    assert(vc[2][0]==0.0 && vc[2][1]==0.0 && vc[2][2]==1.0);

    // scalar multiplication 
    std::cout << "\ntest: scalar scaling" << std::endl;
    std::cout << "scaling by 0.5" << std::endl;
    for(auto& v : vc) v = 0.5*v; 
    for(auto v : vc) std::cout << v << "\t"; std::cout << std::endl;
    assert(vc[0][0]==0.5 && vc[0][1]==0.0 && vc[0][2]==0.0); 
    assert(vc[1][0]==0.0 && vc[1][1]==0.5 && vc[1][2]==0.0); 
    assert(vc[2][0]==0.0 && vc[2][1]==0.0 && vc[2][2]==0.5); 

    // normalization 
    std::cout << "\ntest: normalization" << std::endl;
    std::cout << "scaling by 0.5" << std::endl;
    for(auto& v : vc) v.normalize();
    for(auto v : vc) std::cout << v << "\t"; std::cout << std::endl;

    // vector addition 
    std::cout << "\ntest: addition" << std::endl;
    Vector3d v4 {0,0,0};
    for(auto& v : vc) v4 += v;
    for(auto v : vc) std::cout << v << "\t"; 
    std::cout << " all added ==> " << v4 << std::endl;
    assert(v4[0]==1.0 && v4[1]==1.0 && v4[2]==1.0); 
    
    // vector substraction 
    std::cout << "\ntest: substraction" << std::endl;
    Vector3d v5 {0,0,0};
    for(auto& v : vc) v5 -= v;
    for(auto v : vc) std::cout << v << "\t"; 
    std::cout << " all substracted ==> " << v5 << std::endl;
    assert(v5[0]==-1.0 && v5[1]==-1.0 && v5[2]==-1.0); 

    // unary minus 
    std::cout << "\ntest: unary minus" << std::endl;
    Vector3d v7 {1,2,3};
    Vector3d v8;
    v8 = -v7;
    std::cout << "-" << v7 << " = " << v8 << std::endl;
    assert(v8[0]==-1.0 && v8[1]==-2.0 && v8[2]==-3.0);

    // cross product 
    std::cout << "\ntest: cross product" << std::endl;
    Vector3d v100 {1,0,0};
    Vector3d v010 {0,1,0};
    Vector3d v001 = cross(v100, v010); 
    std::cout << v100 << "x" << v010 << " = " << v001 << std::endl;
    assert(v001[0]==0.0 && v001[1]==0.0 && v001[2]==1.0); 

    // scalar product
    std::cout << "\ntest: dot product" << std::endl;
    double sp1 = dot(v100,v010);
    std::cout << v100 << "." << v010 << " = " << sp1 << std::endl;
    assert(sp1==0); 
    double sp2 = dot(v100,v100);
    std::cout << v100 << "." << v100 << " = " << sp2 << std::endl;
    assert(sp2==1);
}

int main()
{
    test();
	return 0;
}
