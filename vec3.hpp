/*************************************************************************************
*                                  SRL ENGINE                                        *
*                                                                                    *
*                                                                                    *
*    Copyright (C) 2010 Pecatte Baptiste                                             *
*                                                                                    *
*    This program is free software; you can redistribute it and/or                   *
*    modify it under the terms of the GNU Lesser General Public                      *
*    License as published by the Free Software Foundation; either                    *
*    version 2.1 of the License, or (at your option) any later version.              *
*                                                                                    *
*    This program is distributed in the hope that it will be useful,                 *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of                  *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU                *
*    Lesser General Public License for more details.                                 *
*                                                                                    *
*    You should have received a copy of the GNU Lesser General Public                *
*    License along with this program; if not, write to the Free Software             *
*    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA    *
*                                                                                    *
**************************************************************************************/
#ifndef srlHeaderVec3
#define srlHeaderVec3

#include <cmath>

#ifdef srlNamespaceMaths
namespace srl {
#endif

	class vec3 {
		public:
			vec3() : x(0), y(0), z(0) {};
			vec3(double f) : x(f), y(f), z(f) {};
			vec3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {};
			
            vec3  operator-  () {
				return vec3(-x, -y, -z);
			}
			
			
            vec3  operator+  ( const vec3 &v ) {
				return vec3(x + v.x, y + v.y, z + v.z);
			}
            vec3  operator+  ( const double &f ) {
				if(f < length()) return *this;
				double ratio = f/length();
				return (*this) * (1 + ratio);
			}
			
			vec3& operator+= ( const vec3 &v ) {
				x += v.x; y += v.y; z += v.z;
				return *this;
			}
            vec3& operator+= ( const double &f ) {
				double f2 = f;
				if(f < 0.0 && (-f > length() -2.0)) f2 = - length() + 2.0;
				
				// if(ratio <= -1) ratio = -0.9;
				double ratio = f2/length();
				x *= (1+ratio); y *= (1+ratio); z *= (1+ratio);
				return *this;
			}
			
			double increase( const double &f ) {
				double f2 = f;
				if(f < 0.0 && (-f > length() -2.0)) f2 = - length() + 2.0;
				
				double ratio = f2/length();
				x *= (1+ratio); y *= (1+ratio); z *= (1+ratio);
				return f2;
			}
            
			
            vec3  operator-  ( const vec3 &v ) {
				return vec3(x - v.x, y - v.y, z - v.z);
			}
            vec3  operator-  ( const double &f ) {
				return vec3(x - f, y - f, z - f);
			}
			
            vec3& operator-= ( const vec3 &v ) {
				x -= v.x; y -= v.y; z -= v.z;
				return *this;
			}
            vec3& operator-= ( const double &f ) {
				x -= f; y -= f; z -= f;
				return *this;
			}			
            
			
            vec3  operator*  ( const vec3 &v ) {
				return vec3(x * v.x, y * v.y, z * v.z);
			}
            vec3  operator*  ( const double &f ) {
				return vec3(x * f, y * f, z * f);
			}
			
            vec3& operator*= ( const vec3 &v ) {
				x *= v.x; y *= v.y; z *= v.z;
				return *this;
			}
            vec3& operator*= ( const double &f ) {
				x *= f; y *= f; z *= f;
				return *this;
			}
            
			
            vec3  operator/  ( const vec3 &v ) {
				return vec3(x / v.x, y / v.y, z / v.z);
			}
            vec3  operator/  ( const double &f ) {
				return vec3(x / f, y / f, z / f);
			}
			
            vec3& operator/= ( const vec3 &v ) {
				x /= v.x; y /= v.y; z /= v.z;
				return *this;
			}
            vec3& operator/= ( const double &f ) {
				x /= f; y /= f; z /= f;
				return *this;
			}
            
			
            bool operator!= ( const vec3 &v ) {
				return (x != v.x) || (y != v.y) || (z != v.z);
			}
			bool operator== ( const vec3 &v ) {
				return (x == v.x) || (y == v.y) || (z == v.z);
			}
			
			bool operator< ( const vec3 &v ) {
				return (x < v.x) && (y < v.y) && (z < v.z);
			}
			bool operator<= ( const vec3 &v ) {
				return (x <= v.x) && (y <= v.y) && (z <= v.z);
			}
			bool operator> ( const vec3 &v ) {
				return (x > v.x) && (y > v.y) && (z > v.z);
			}
			bool operator>= ( const vec3 &v ) {
				return (x >= v.x) && (y >= v.y) && (z >= v.z);
			}
			
			
			double&	operator[] ( const int i ) {
				switch(i) {
					case 0: return x; break;
					case 1: return y; break;
					case 2: return z; break;
				}
			}
			
			
			double length() {
				return sqrt(x*x + y*y + z*z);
			}
			
			double sqlen() {
				return x*x + y*y + z*z;
			}
			
			void normalize() {
				double l = length();
				if(l > 0.0) {
					x /= l;
					y /= l;
					z /= l;
				}
			}
			
			void cross( const vec3 &v ) {
				x = y * v.z - z * v.y;
				y = z * v.x - x * v.z;
				z = x * v.y - y * v.x;
			}
			
			double dot(const vec3 &v ) { 
				return x * v.x + y * v.y + z * v.z;
			}
			
		double x;
		double y;
		double z;
	};
	
	inline double length(vec3 v) {
		return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	}
	
	inline void normalize(vec3 v) {
		v.normalize();
	}
	
	inline vec3 normalized(vec3 v) {
		double l = length(v);
		if(l > 0.0) return vec3(v.x/l, v.y/l, v.z/l);
		else return vec3();
	}
	
	inline vec3 cross(vec3 v, vec3 u) {
		return vec3(v.y * u.z - v.z * u.y,
					v.z * u.x - v.x * u.z,
					v.x * u.y - v.y * u.x);
	}
	
	inline double dot(vec3 v, vec3 u) {
		return v.x * u.x + v.y * u.y + v.z * u.z;
	}
	
#ifdef srlNamespaceMaths	
};
#endif

#endif
