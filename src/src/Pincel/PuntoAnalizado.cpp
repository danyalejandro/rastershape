#include "PuntoAnalizado.h"

ostream& operator << (ostream &o, const PuntoAnalizado &pun) {
	o << "{ " << pun.num << ", " << pun.pos << ", " << rad2deg(pun.angulo) << "°, " << (int)pun.codValido << " }";
	return o;
}