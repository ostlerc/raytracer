//Written by Chad Ostler
//Uses rply.h which was *not* written by Chad Ostler

#ifndef __PLY_WRAPPER_H__
#define __PLY_WRAPPER_H__

#include "rply.h"
#include "Vector.h"
#include <vector>

namespace ply
{
    class plyFile
    {
        public:
            std::vector<long double> getVertices() { return _vertices; }
            std::vector<unsigned> getElements() { return _elements; }

            std::vector<long double> getNormals();

            Vector normal(int i);
            Vector vertex(int i);

            static plyFile parseFile(const char* fileName);

        private:
            std::vector<long double> _vertices;
            std::vector<unsigned> _elements;

            std::vector<long double> _normals;
    };
}

#endif // __PLY_WRAPPER_H__
