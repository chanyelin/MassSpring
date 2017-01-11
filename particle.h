#pragma once
#include "library.h"
const double dT = 0.02; // 0.01 second per one frame
const double g = 9.8; // gravity
const double k = 0.1; // spring constant
const int N = 7; // N*N = number of particles
const double DEG2RAD = 3.141592/180;

struct Particle{
	V2 p; // position
	V2 F; // Force
	V2 v; // velocity
	double mass;
	int id;
	int fixed; // if fixed, F = v = 0
	vector<Particle*> connect;
	vector<double> restLength;
	Particle(){};
	Particle(V2 _p, V2 _v, double _mass, int _id, int _fixed = 0):p(_p),v(_v),mass(_mass),id(_id),fixed(_fixed){};
	void move(){ // change position to next frame
		F = V2(0,-g);
		for (int i=0;i<connect.size();i++){
			Particle* q = connect[i];
			double rest = restLength[i];
			V2 diff = q->p - p;
			double len = sqrt(diff.dot(diff));
			if (len < 1e-6) continue;

			V2 vec = diff / len;
			V2 f = -k * vec * (rest - len);

			F = F + f;
		}

		V2 a = F / mass; // acceleration

		// --------------------- //
		V2 next_p = p + v * dT;
		V2 next_v = v + a * dT;
		// --------------------- //

		if (next_p(1) < 0){
			// TODO : handle collision
			next_p = p - v*dT;
			next_v(1) = -next_v(1);
		}

		p = next_p;
		v = next_v;
	}
};

void push_edge(Particle *x, Particle *y, double rest){
	x->connect.push_back(y);
	x->restLength.push_back(rest);

	y->connect.push_back(x);
	y->restLength.push_back(rest);
}

Particle particles[N*N];
int dir1[4][2]={{0,1},{1,0},{0,-1},{-1,0}};
int dir2[4][2]={{1,1},{1,-1},{-1,-1},{-1,1}};
void Initialize(){
	for (int t=0;t<N*N;t++){
		int i = t/N, j = t%N;
		particles[t]=Particle(V2(-150,50) + V2(50,0)*i + V2(0,50)*j, V2(0,0), 1, t, 0);
		for (int k=0;k<4;k++){
			int i2 = i + dir1[k][0], j2 = j + dir1[k][1];
			if (i2<0 || j2<0 || i2>=N || j2>=N) continue;
			int t2 = i2 * N + j2;
			push_edge(&particles[t], &particles[t2], 50);
		}
		for (int k=0;k<4;k++){
			int i2 = i + dir2[k][0], j2 = j + dir2[k][1];
			if (i2<0 || j2<0 || i2>=N || j2>=N) continue;
			int t2 = i2 * N + j2;
			push_edge(&particles[t], &particles[t2], 50 * sqrt(2.0));
		}
	}
}


void drawCircle(V2 p, double radius){
	glBegin(GL_LINE_LOOP);
	int M=360;
	for (int i=0;i<M;i++){
		double deg = (i * 360 / M) * DEG2RAD;
		V2P((p + V2(cos(deg)*radius, sin(deg)*radius)));
	}
	glEnd();
}

void drawParticles(){
	for (int i=0;i<N*N;i++){
		drawCircle(particles[i].p, 5.0);
		for (int j=0;j<particles[i].connect.size();j++){
			Particle q = *particles[i].connect[j];
			glBegin(GL_LINE_STRIP);
			V2P(particles[i].p);
			V2P(q.p);
			glEnd();
		}
	}
}

void nextFrame(){
	for (int i=0;i<N*N;i++){
		particles[i].move();
	}
}