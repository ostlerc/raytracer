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
            std::vector<double> getVertices() { return _vertices; }
            std::vector<unsigned> getElements() { return _elements; }

            std::vector<double> getNormals();

            static plyFile parseFile(const char* fileName);

        private:
            std::vector<double> _vertices;
            std::vector<unsigned> _elements;

            std::vector<double> _normals;

            Vector vertex(int i);
    };
}

#endif // __PLY_WRAPPER_H__
