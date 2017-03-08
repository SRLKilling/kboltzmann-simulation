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
#ifndef srlHeaderVec2
#define srlHeaderVec2

#include <cmath>

#ifdef srlNamespaceMaths
namespace srl {
#endif

	class vec2 {
		public:
			vec2() : x(0), y(0) {};
			vec2(double f) : x(f), y(f) {};
			vec2(double x, double y) : x(x), y(y) {};
			
            vec2  operator-  () {
				return vec2(-x, -y);
			}
			
			
            vec2  operator+  (const vec2 &v) {
				return vec2(x + v.x, y + v.y);
			}
            vec2  operator+  (const double &f) {
				return vec2(x + f, y + f);
			}
			
			vec2& operator+= ( const vec2 &v ) {
				x += v.x; y += v.y;
				return *this;
			}
            vec2& operator+= ( const double &f ) {
				x += f; y += f;
				return *this;
			}
            
			
            vec2  operator-(const vec2 &v ) {
				return vec2(x - v.x, y - v.y);
			}
            vec2  operator-(const double &f) {
				return vec2(x - f, y - f);
			}
			
            vec2& operator-=(const vec2 &v) {
				x -= v.x; y -= v.y;
				return *this;
			}
            vec2& operator-=(const double &f) {
				x -= f; y -= f;
				return *this;
			}			
            
			
            vec2  operator*  ( const vec2 &v ) {
				return vec2(x * v.x, y * v.y);
			}
            vec2  operator*  ( const double &f ) {
				return vec2(x * f, y * f);
			}
			
            vec2& operator*= ( const vec2 &v ) {
				x *= v.x; y *= v.y;
				return *this;
			}
            vec2& operator*= ( const double &f ) {
				x *= f; y *= f;
				return *this;
			}
            
			
            vec2  operator/  ( const vec2 &v ) {
				return vec2(x / v.x, y / v.y);
			}
            vec2  operator/  ( const double &f ) {
				return vec2(x / f, y / f);
			}
			
            vec2& operator/= ( const vec2 &v ) {
				x /= v.x; y /= v.y;
				return *this;
			}
            vec2& operator/= ( const double &f ) {
				x /= f; y /= f;
				return *this;
			}
            
			
            bool operator!= ( const vec2 &v ) {
				return (x != v.x) || (y != v.y);
			}
			bool operator== ( const vec2 &v ) {
				return (x == v.x) || (y == v.y);
			}
			
			
			double&	operator[] ( const int i ) {
				switch(i) {
					case 0: return x; break;
					case 1: return y; break;
				}
			}
			
			double sqlen() {
				return (x*x + y*y);
			}
			
			double length() {
				return sqrt(x*x + y*y);
			}
			
			void normalize() {
				double l = length();
				x /= l; y /= l;;
			}
			
			double dot(const vec2 &v ) { 
				return x * v.x + y * v.y;
			}
			
		double x;
		double y;
	};
	
	inline double length(vec2 v) {
		return sqrt(v.x*v.x + v.y*v.y);
	}
	
	inline void normalize(vec2 v) {
		v.normalize();
	}
	
	inline vec2 normalized(vec2 v) {
		double l = length(v);
		return vec2(v.x/l, v.y/l);
	}
	
	inline double dot(vec2 v, vec2 u) {
		return v.x * u.x + v.y * u.y;
	}
	
#ifdef srlNamespaceMaths	
};
#endif

#endif
