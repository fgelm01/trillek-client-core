#include "rendering/dual_marching_cubes_render_algorithm.h"
#include "rendering/marching_cubes_render_algorithm.h"
#include "services/graphics_service.h"
#include <SFML/OpenGL.hpp>
#include "math/axis_aligned_box.h"
namespace trillek
{

dual_marching_cubes_render_algorithm::dual_marching_cubes_render_algorithm()
{
    //ctor
}

dual_marching_cubes_render_algorithm::~dual_marching_cubes_render_algorithm()
{
    //dtor
}

bool check_if_ptr_unique(intptr_t& p,
                         std::array<intptr_t,4>& a,
                         unsigned char& num_ptrs)
{
    for(unsigned char i=0; i<num_ptrs;++i) {
        if(a[i]==p) {
            return false;
        }else if(i==num_ptrs-1) {
            a[num_ptrs]=p;
            num_ptrs++;
            return true;
        }
    }
}

unsigned char count_unique_ptrs(intptr_t p0,intptr_t p1,
                                intptr_t p2,intptr_t p3) {
    unsigned char num_ptrs=1;
    std::array<intptr_t,4> unique_ptrs;
    unique_ptrs[0]=p0;
    check_if_ptr_unique(p1,unique_ptrs,num_ptrs);
    check_if_ptr_unique(p2,unique_ptrs,num_ptrs);
    check_if_ptr_unique(p3,unique_ptrs,num_ptrs);
    return num_ptrs;
}

void process_dual_cell_face(voxel_octree* n0,
                            voxel_octree* n1,
                            std::size_t cube_num,
                            std::shared_ptr<mesh_data> model,
                            axis_aligned_box& center_box,
                            std::size_t type,
                            voxel_data* data)
{
    if(cube_num!=0&&cube_num!=255)
    {
        float size=1.0f;
        if(n0!=n1)
        {
            std::size_t min_size=std::min(n0->get_size().x,
                                          n1->get_size().x);
            if(min_size-1.0f>0)
            {
                axis_aligned_box face_box=center_box;
                face_box.set_widths(type&0x1?(float)min_size-size:size,
                                    type&0x2?(float)min_size-size:size,
                                    type&0x4?(float)min_size-size:size);
                face_box.translate(type&0x1?min_size/2.0f:0,
                                   type&0x2?min_size/2.0f:0,
                                   type&0x4?min_size/2.0f:0);
                marching_cubes_render_algorithm::step(face_box,cube_num,model,data);
            }
        }
    }
}

void dual_marching_cubes_render_algorithm::create_dual_cells(voxel_octree* n0,
                                                             voxel_octree* n1,
                                                             voxel_octree* n2,
                                                             voxel_octree* n3,
                                                             voxel_octree* n4,
                                                             voxel_octree* n5,
                                                             voxel_octree* n6,
                                                             voxel_octree* n7,
                                             std::shared_ptr<mesh_data> model,
                                                             voxel_data* data)
{
    std::array<voxel,8> v = {n0->get_voxel(),n1->get_voxel(),
                             n4->get_voxel(),n5->get_voxel(),
                             n2->get_voxel(),n3->get_voxel(),
                             n6->get_voxel(),n7->get_voxel()};
    std::size_t cube_num=
    marching_cubes_render_algorithm::calculate_cube_num(v[0],v[1],v[2],v[3],
                                                        v[4],v[5],v[6],v[7]);

    if(cube_num==0||cube_num==255)
        return;
    std::array<voxel_octree*,8> n={n0,n1,n4,n5,n2,n3,n6,n7};
    std::size_t min_size_num=0;
	std::size_t min_size=n0->get_size().x;
	std::size_t max_size=min_size;
    for(int i=1; i<8;++i)
    {
        if(n[i]->get_size().x<min_size)
        {
            min_size=n[i]->get_size().x;
            min_size_num=i;
        }
        if(n[i]->get_size().x>max_size)
        {
            max_size=n[i]->get_size().x;
        }
    }
    float size=1.0f;

    auto new_offset=n[min_size_num]->get_child_offset_by_index(min_size_num);
    auto new_center =n[min_size_num]->get_offset()
                     -new_offset*2
                     -vector3d<float>(size/2,size/2,size/2);

    axis_aligned_box center_box(new_center,vector3d<std::size_t>(size,
                                                                 size,
                                                                 size));

    marching_cubes_render_algorithm::step(center_box,cube_num,model,data);
    if(max_size>size)
    {
        std::size_t min_size_x=0,min_size_y=0,min_size_z=0;
        cube_num=
    marching_cubes_render_algorithm::calculate_cube_num(v[1],v[1],v[3],v[3],
                                                        v[5],v[5],v[7],v[7]);
        if(cube_num!=0&&cube_num!=255)
        {
            if(count_unique_ptrs((intptr_t)n[1],(intptr_t)n[3],
                                 (intptr_t)n[5],(intptr_t)n[7])>2)
            {
                min_size_x=n[1]->get_size().x;
                if(n[3]->get_size().x < min_size_x)
                    min_size_x=n[3]->get_size().x;
                if(n[5]->get_size().x < min_size_x)
                    min_size_x=n[5]->get_size().x;
                if(n[7]->get_size().x < min_size_x)
                    min_size_x=n[7]->get_size().x;
                if(min_size_x>size)
                {
                    // X-Axis Edge
                    axis_aligned_box x_edge_box=center_box;
                    x_edge_box.set_widths((float)min_size_x-size,size,size);
                    x_edge_box.translate(min_size_x/2.0f,0,0);
                    marching_cubes_render_algorithm::step(x_edge_box,cube_num,model,data);
                }
            }
        }

        cube_num=
    marching_cubes_render_algorithm::calculate_cube_num(v[2],v[3],v[2],v[3],
                                                        v[6],v[7],v[6],v[7]);
        if(cube_num!=0&&cube_num!=255)
        {
            if(count_unique_ptrs((intptr_t)n[2],(intptr_t)n[3],
                                 (intptr_t)n[6],(intptr_t)n[7])>2)
            {
                min_size_y=n[2]->get_size().x;
                if(n[3]->get_size().x < min_size_y)
                    min_size_y=n[3]->get_size().x;
                if(n[6]->get_size().x < min_size_y)
                    min_size_y=n[6]->get_size().x;
                if(n[7]->get_size().x < min_size_y)
                    min_size_y=n[7]->get_size().x;
                if(min_size_y>size)
                {
                    // Y-Axis Edge
                    axis_aligned_box y_edge_box=center_box;
                    y_edge_box.set_widths(size,(float)min_size_y-size,size);
                    y_edge_box.translate(0,min_size_y/2.0f,0);
                    marching_cubes_render_algorithm::step(y_edge_box,cube_num,model,data);
                }
            }
        }

        cube_num=
    marching_cubes_render_algorithm::calculate_cube_num(v[4],v[5],v[6],v[7],
                                                        v[4],v[5],v[6],v[7]);
        if(cube_num!=0&&cube_num!=255)
        {
            if(count_unique_ptrs((intptr_t)n[4],(intptr_t)n[5],
                                 (intptr_t)n[6],(intptr_t)n[7])>2)
            {
                min_size_z=n[4]->get_size().x;
                if(n[5]->get_size().x < min_size_z)
                    min_size_z=n[5]->get_size().x;
                if(n[6]->get_size().x < min_size_z)
                    min_size_z=n[6]->get_size().x;
                if(n[7]->get_size().x < min_size_z)
                    min_size_z=n[7]->get_size().x;
                if(min_size_z>size)
                {
                    // Z-Axis Edge
                    axis_aligned_box z_edge_box=center_box;
                    z_edge_box.set_widths(size,size,(float)min_size_z-size);
                    z_edge_box.translate(0,0,min_size_z/2.0f);
                    marching_cubes_render_algorithm::step(z_edge_box,cube_num,model,data);
                }
            }
        }

        // XY-Face
        cube_num=
    marching_cubes_render_algorithm::calculate_cube_num(v[3],v[3],v[3],v[3],
                                                        v[7],v[7],v[7],v[7]);
        if(min_size_x != 0 && min_size_y != 0) {
            process_dual_cell_face(n[3],n[7],cube_num,model,center_box,0x3,data);
        }

        // YZ-Face
        cube_num=
    marching_cubes_render_algorithm::calculate_cube_num(v[6],v[7],v[6],v[7],
                                                        v[6],v[7],v[6],v[7]);
        if(min_size_y != 0 && min_size_z != 0) {
            process_dual_cell_face(n[6],n[7],cube_num,model,center_box,0x6,data);
        }

        // XZ-Face
        cube_num=
    marching_cubes_render_algorithm::calculate_cube_num(v[5],v[5],v[7],v[7],
                                                        v[5],v[5],v[7],v[7]);
        if(min_size_x != 0 && min_size_z != 0) {
            process_dual_cell_face(n[5],n[7],cube_num,model,center_box,0x5,data);
        }
    }
}

void dual_marching_cubes_render_algorithm::vertProc(voxel_octree* n0,
                                                    voxel_octree* n1,
                                                    voxel_octree* n2,
                                                    voxel_octree* n3,
                                                    voxel_octree* n4,
                                                    voxel_octree* n5,
                                                    voxel_octree* n6,
                                                    voxel_octree* n7,
                                            std::shared_ptr<mesh_data> model,
                                                    voxel_data* data)
{
    if(n0->has_children()||n1->has_children()||
       n2->has_children()||n3->has_children()||
       n4->has_children()||n5->has_children()||
       n6->has_children()||n7->has_children())
    {
        voxel_octree* c0 = n0->has_children() ? n0->get_child(1,1,1):n0;
        voxel_octree* c1 = n1->has_children() ? n1->get_child(0,1,1):n1;
        voxel_octree* c2 = n2->has_children() ? n2->get_child(1,1,0):n2;
        voxel_octree* c3 = n3->has_children() ? n3->get_child(0,1,0):n3;
        voxel_octree* c4 = n4->has_children() ? n4->get_child(1,0,1):n4;
        voxel_octree* c5 = n5->has_children() ? n5->get_child(0,0,1):n5;
        voxel_octree* c6 = n6->has_children() ? n6->get_child(1,0,0):n6;
        voxel_octree* c7 = n7->has_children() ? n7->get_child(0,0,0):n7;
        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model,data);
    }else
    {
        create_dual_cells(n0,n1,n2,n3,n4,n5,n6,n7,model,data);
    }
}

void dual_marching_cubes_render_algorithm::edgeProcX( voxel_octree* n0,
                                                      voxel_octree* n1,
                                                      voxel_octree* n2,
                                                      voxel_octree* n3,
                                            std::shared_ptr<mesh_data> model,
                                                      voxel_data* data)
{
    if(n0->has_children()||n1->has_children()||
       n2->has_children()||n3->has_children())
    {
        voxel_octree* c0 = n0->has_children() ? n0->get_child(0,1,1):n0;
        voxel_octree* c1 = n0->has_children() ? n0->get_child(1,1,1):n0;
        voxel_octree* c2 = n1->has_children() ? n1->get_child(0,1,0):n1;
        voxel_octree* c3 = n1->has_children() ? n1->get_child(1,1,0):n1;
        voxel_octree* c4 = n2->has_children() ? n2->get_child(0,0,1):n2;
        voxel_octree* c5 = n2->has_children() ? n2->get_child(1,0,1):n2;
        voxel_octree* c6 = n3->has_children() ? n3->get_child(0,0,0):n3;
        voxel_octree* c7 = n3->has_children() ? n3->get_child(1,0,0):n3;

        edgeProcX(c0,c2,c4,c6,model,data);
        edgeProcX(c1,c3,c5,c7,model,data);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model,data);
    }
}

void dual_marching_cubes_render_algorithm::edgeProcY( voxel_octree* n0,
                                                      voxel_octree* n1,
                                                      voxel_octree* n2,
                                                      voxel_octree* n3,
                                            std::shared_ptr<mesh_data> model,
                                                      voxel_data* data)
{
    if(n0->has_children()||n1->has_children()||
       n2->has_children()||n3->has_children())
    {
        voxel_octree* c0 = n0->has_children() ? n0->get_child(1,0,1):n0;
        voxel_octree* c1 = n1->has_children() ? n1->get_child(0,0,1):n1;
        voxel_octree* c2 = n2->has_children() ? n2->get_child(1,0,0):n2;
        voxel_octree* c3 = n3->has_children() ? n3->get_child(0,0,0):n3;
        voxel_octree* c4 = n0->has_children() ? n0->get_child(1,1,1):n0;
        voxel_octree* c5 = n1->has_children() ? n1->get_child(0,1,1):n1;
        voxel_octree* c6 = n2->has_children() ? n2->get_child(1,1,0):n2;
        voxel_octree* c7 = n3->has_children() ? n3->get_child(0,1,0):n3;

        edgeProcY(c0,c1,c2,c3,model,data);
        edgeProcY(c4,c5,c6,c7,model,data);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model,data);
    }
}

void dual_marching_cubes_render_algorithm::edgeProcZ( voxel_octree* n0,
                                                      voxel_octree* n1,
                                                      voxel_octree* n2,
                                                      voxel_octree* n3,
                                            std::shared_ptr<mesh_data> model,
                                                      voxel_data* data)
{
    if(n0->has_children()||n1->has_children()||
       n2->has_children()||n3->has_children())
    {
        voxel_octree* c0 = n0->has_children() ? n0->get_child(1,1,0):n0;
        voxel_octree* c1 = n1->has_children() ? n1->get_child(0,1,0):n1;
        voxel_octree* c2 = n0->has_children() ? n0->get_child(1,1,1):n0;
        voxel_octree* c3 = n1->has_children() ? n1->get_child(0,1,1):n1;
        voxel_octree* c4 = n2->has_children() ? n2->get_child(1,0,0):n2;
        voxel_octree* c5 = n3->has_children() ? n3->get_child(0,0,0):n3;
        voxel_octree* c6 = n2->has_children() ? n2->get_child(1,0,1):n2;
        voxel_octree* c7 = n3->has_children() ? n3->get_child(0,0,1):n3;

        edgeProcZ(c0,c1,c4,c5,model,data);
        edgeProcZ(c2,c3,c6,c7,model,data);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model,data);
    }
}

void dual_marching_cubes_render_algorithm::faceProcXY(voxel_octree* n0,
                                                      voxel_octree* n1,
                                            std::shared_ptr<mesh_data> model,
                                                      voxel_data* data)
{
    if(n0->has_children()||n1->has_children())
    {
        voxel_octree* c0 = n0->has_children() ? n0->get_child(0,0,1):n0;
        voxel_octree* c1 = n0->has_children() ? n0->get_child(1,0,1):n0;
        voxel_octree* c2 = n1->has_children() ? n1->get_child(0,0,0):n1;
        voxel_octree* c3 = n1->has_children() ? n1->get_child(1,0,0):n1;
        voxel_octree* c4 = n0->has_children() ? n0->get_child(0,1,1):n0;
        voxel_octree* c5 = n0->has_children() ? n0->get_child(1,1,1):n0;
        voxel_octree* c6 = n1->has_children() ? n1->get_child(0,1,0):n1;
        voxel_octree* c7 = n1->has_children() ? n1->get_child(1,1,0):n1;

        faceProcXY(c0,c2,model,data);
        faceProcXY(c1,c3,model,data);
        faceProcXY(c4,c6,model,data);
        faceProcXY(c5,c7,model,data);

        edgeProcX(c0,c2,c4,c6,model,data);
        edgeProcX(c1,c3,c5,c7,model,data);

        edgeProcY(c0,c1,c2,c3,model,data);
        edgeProcY(c4,c5,c6,c7,model,data);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model,data);
    }
}

void dual_marching_cubes_render_algorithm::faceProcYZ(voxel_octree* n0,
                                                      voxel_octree* n1,
                                            std::shared_ptr<mesh_data> model,
                                                      voxel_data* data)
{
    if(n0->has_children()||n1->has_children())
    {
        voxel_octree* c0 = n0->has_children() ? n0->get_child(1,0,0):n0;
        voxel_octree* c1 = n1->has_children() ? n1->get_child(0,0,0):n1;
        voxel_octree* c2 = n0->has_children() ? n0->get_child(1,0,1):n0;
        voxel_octree* c3 = n1->has_children() ? n1->get_child(0,0,1):n1;
        voxel_octree* c4 = n0->has_children() ? n0->get_child(1,1,0):n0;
        voxel_octree* c5 = n1->has_children() ? n1->get_child(0,1,0):n1;
        voxel_octree* c6 = n0->has_children() ? n0->get_child(1,1,1):n0;
        voxel_octree* c7 = n1->has_children() ? n1->get_child(0,1,1):n1;

        faceProcYZ(c0,c1,model,data);
        faceProcYZ(c2,c3,model,data);
        faceProcYZ(c4,c5,model,data);
        faceProcYZ(c6,c7,model,data);

        edgeProcY(c0,c1,c2,c3,model,data);
        edgeProcY(c4,c5,c6,c7,model,data);

        edgeProcZ(c0,c1,c4,c5,model,data);
        edgeProcZ(c2,c3,c6,c7,model,data);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model,data);
    }
}

void dual_marching_cubes_render_algorithm::faceProcXZ(voxel_octree* n0,
                                                      voxel_octree* n1,
                                            std::shared_ptr<mesh_data> model,
                                                      voxel_data* data)
{
    if(n0->has_children()||n1->has_children())
    {
        voxel_octree* c0 = n0->has_children() ? n0->get_child(0,1,0):n0;
        voxel_octree* c1 = n0->has_children() ? n0->get_child(1,1,0):n0;
        voxel_octree* c2 = n0->has_children() ? n0->get_child(0,1,1):n0;
        voxel_octree* c3 = n0->has_children() ? n0->get_child(1,1,1):n0;
        voxel_octree* c4 = n1->has_children() ? n1->get_child(0,0,0):n1;
        voxel_octree* c5 = n1->has_children() ? n1->get_child(1,0,0):n1;
        voxel_octree* c6 = n1->has_children() ? n1->get_child(0,0,1):n1;
        voxel_octree* c7 = n1->has_children() ? n1->get_child(1,0,1):n1;

        faceProcXZ(c0,c4,model,data);
        faceProcXZ(c1,c5,model,data);
        faceProcXZ(c2,c6,model,data);
        faceProcXZ(c3,c7,model,data);

        edgeProcX(c0,c2,c4,c6,model,data);
        edgeProcX(c1,c3,c5,c7,model,data);

        edgeProcZ(c0,c1,c4,c5,model,data);
        edgeProcZ(c2,c3,c6,c7,model,data);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model,data);
    }
}

void dual_marching_cubes_render_algorithm::nodeProc(voxel_octree* n,
                                            std::shared_ptr<mesh_data> model,
                                                    voxel_data* data)
{
    if(n->has_children())
    {
        // Visit all eight children
        for(int i=0; i<8; ++i)
            nodeProc(n->get_child(i),model,data);

        faceProcXY(n->get_child(0,0,0),n->get_child(0,0,1),model,data);
        faceProcXY(n->get_child(1,0,0),n->get_child(1,0,1),model,data);
        faceProcXY(n->get_child(0,1,0),n->get_child(0,1,1),model,data);
        faceProcXY(n->get_child(1,1,0),n->get_child(1,1,1),model,data);

        faceProcYZ(n->get_child(0,0,0),n->get_child(1,0,0),model,data);
        faceProcYZ(n->get_child(0,0,1),n->get_child(1,0,1),model,data);
        faceProcYZ(n->get_child(0,1,0),n->get_child(1,1,0),model,data);
        faceProcYZ(n->get_child(0,1,1),n->get_child(1,1,1),model,data);

        faceProcXZ(n->get_child(0,0,0),n->get_child(0,1,0),model,data);
        faceProcXZ(n->get_child(1,0,0),n->get_child(1,1,0),model,data);
        faceProcXZ(n->get_child(0,0,1),n->get_child(0,1,1),model,data);
        faceProcXZ(n->get_child(1,0,1),n->get_child(1,1,1),model,data);

        edgeProcX(n->get_child(0,0,0),n->get_child(0,0,1),
                  n->get_child(0,1,0),n->get_child(0,1,1),model,data);
        edgeProcX(n->get_child(1,0,0),n->get_child(1,0,1),
                  n->get_child(1,1,0),n->get_child(1,1,1),model,data);

        edgeProcY(n->get_child(0,0,0),n->get_child(1,0,0),
                  n->get_child(0,0,1),n->get_child(1,0,1),model,data);
        edgeProcY(n->get_child(0,1,0),n->get_child(1,1,0),
                  n->get_child(0,1,1),n->get_child(1,1,1),model,data);

        edgeProcZ(n->get_child(0,0,0),n->get_child(1,0,0),
                  n->get_child(0,1,0),n->get_child(1,1,0),model,data);
        edgeProcZ(n->get_child(0,0,1),n->get_child(1,0,1),
                  n->get_child(0,1,1),n->get_child(1,1,1),model,data);

        vertProc(n->get_child(0,0,0),n->get_child(1,0,0),
                 n->get_child(0,0,1),n->get_child(1,0,1),
                 n->get_child(0,1,0),n->get_child(1,1,0),
                 n->get_child(0,1,1),n->get_child(1,1,1),model,data);
    }
}

void dual_marching_cubes_render_algorithm::process(voxel_model* node,graphics_service* service)
{
    std::cerr << "Rendering start" << std::endl;
    std::shared_ptr<mesh_data> model = std::make_shared<mesh_data>();

    voxel_data* r_d = node->get_render_data();
    voxel_octree* octree = voxel_octree::convert(r_d);
    nodeProc(octree,model,r_d);
    service->register_model((uintptr_t)node,model);
    std::cerr << "Rendering end" << std::endl;
}

}
