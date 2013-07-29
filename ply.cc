//Written by Chad Ostler
//Uses rply.h which was *not* written by Chad Ostler

#include "ply.h"

#include <iostream>
#include <vector>

namespace ply
{
    int vertex_cb(p_ply_argument argument)
    {
        std::vector<double> *v = NULL;
        ply_get_argument_user_data(argument, (void**)&v, NULL);
        double d = ply_get_argument_value(argument);
        v->push_back(d);

        return 1;
    }

    int face_cb(p_ply_argument argument)
    {
        long length, value_index;
        std::vector<unsigned> *elements = NULL;

        ply_get_argument_property(argument, NULL, &length, &value_index);
        ply_get_argument_user_data(argument, (void**)&elements, NULL);

        if (value_index >= 0)
        {
            unsigned e = ply_get_argument_value(argument);
            elements->push_back(e);
        }

        return 1;
    }

    plyFile plyFile::parseFile(const char* fileName)
    {
        long nvertices, nelements, nnormals;
        plyFile f;
        p_ply ply = ply_open(fileName, NULL, 0, NULL);
        if (!ply) return f;
        if (!ply_read_header(ply)) return f;

        nelements = ply_set_read_cb(ply, "face", "vertex_indices", face_cb, &f._elements, 0);
        nvertices = ply_set_read_cb(ply, "vertex", "x",  vertex_cb, &f._vertices, 0);
                    ply_set_read_cb(ply, "vertex", "y",  vertex_cb, &f._vertices, 0);
                    ply_set_read_cb(ply, "vertex", "z",  vertex_cb, &f._vertices, 0);

        nnormals  = ply_set_read_cb(ply, "vertex", "nx", vertex_cb, &f._normals,  0);
                    ply_set_read_cb(ply, "vertex", "ny", vertex_cb, &f._normals,  0);
                    ply_set_read_cb(ply, "vertex", "nz", vertex_cb, &f._normals,  0);

        if (!ply_read(ply)) return f;
        ply_close(ply);

        if(f._vertices.size()/3. != nvertices)
            std::cerr << "parsing file error. Not all vertices are accounted for. Expected: " << nvertices*3. << " Actual: " << f._vertices.size() << std::endl;

        if(f._elements.size()/3. != nelements)
            std::cerr << "parsing file error. Not all elements are accounted for. Expected: " << nelements*3. << " Actual: " << f._elements.size() << std::endl;

        if(nnormals > 0. && f._normals.size()/3. != nnormals)
            std::cerr << "parsing file error. Not all normals are accounted for. Expected: " << nnormals*3. << " Actual: " << f._normals.size() << std::endl;

        return f;
    }

    double area(Vector a, Vector b, Vector c)
    {
        Vector e1(b.x() - a.x(), b.y()- a.y(), b.z() - a.z());
        Vector e2(c.x() - a.x(), c.y()- a.y(), c.z() - a.z());
        Vector e3 = Cross(e1,e2);
        return e3.length() / 2.0;
    }

    std::vector<double> plyFile::getNormals()
    {
        if(_vertices.size() == 0 || _elements.size() == 0)
        {
            std::cerr << "no verticies or elements, normals cannot be calculated" << std::endl;
            return _normals;
        }

        if(_elements.size() % 3 != 0)
        {
            std::cerr << "normals cannot be calculated, elements are not a multiple of 3" << std::endl;
            return _normals;
        }

        if(_normals.size() != 0)
            return _normals;

        //calc normals

        std::cout << "calculating normals..." << std::flush;
        std::vector<Vector> normals;

        normals.resize(_vertices.size()/3);

        for(unsigned i = 0; i < normals.size(); i++)
            normals[i] = Vector(0,0,0);

        //loop through faces, calculate normals, add normals to vertex normals
        for(unsigned i = 0; i < _elements.size(); i+=3)
        {
            double j = _elements[i];
            double k = _elements[i+1];
            double l = _elements[i+2];

            Vector a = vertex(k) - vertex(j);
            Vector b = vertex(l) - vertex(k);

            double _area = area(vertex(j), vertex(k), vertex(l));

            Vector faceNormal = Cross(a,b)*_area;

            normals[j] += faceNormal;
            normals[k] += faceNormal;
            normals[l] += faceNormal;
        }

        _normals.resize(_vertices.size());

        for(unsigned i = 0; i < normals.size(); i++)
        {
            normals[i].normalize();

            _normals[3*i]   = normals[i].x();
            _normals[3*i+1] = normals[i].y();
            _normals[3*i+2] = normals[i].z();
            //std::cout << normals[i].x << " " << normals[i].y << " " << normals[i].z << std::endl;
        }

        std::cout << "done" << std::endl;
        return _normals;
    }

    Vector plyFile::vertex(int i)
    {
        i*=3;
        return Vector(_vertices[i], _vertices[i+1], _vertices[i+2]);
    }
}
