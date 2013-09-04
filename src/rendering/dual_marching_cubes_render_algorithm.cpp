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

void render_box(vector3d<float> p0,vector3d<float> p1,
                vector3d<float> p2,vector3d<float> p3,
                vector3d<float> p4,vector3d<float> p5,
                vector3d<float> p6,vector3d<float> p7)
{
    glBegin(GL_QUADS);
        glVertex3f(p0.x,p0.y,p0.z);
        glVertex3f(p1.x,p1.y,p1.z);
        glVertex3f(p4.x,p4.y,p4.z);
        glVertex3f(p5.x,p5.y,p5.z);

        glVertex3f(p2.x,p2.y,p2.z);
        glVertex3f(p3.x,p3.y,p3.z);
        glVertex3f(p6.x,p6.y,p6.z);
        glVertex3f(p7.x,p7.y,p7.z);

        glVertex3f(p0.x,p0.y,p0.z);
        glVertex3f(p1.x,p1.y,p1.z);
        glVertex3f(p2.x,p2.y,p2.z);
        glVertex3f(p3.x,p3.y,p3.z);

        glVertex3f(p4.x,p4.y,p4.z);
        glVertex3f(p5.x,p5.y,p5.z);
        glVertex3f(p6.x,p6.y,p6.z);
        glVertex3f(p7.x,p7.y,p7.z);

        glVertex3f(p0.x,p0.y,p0.z);
        glVertex3f(p2.x,p2.y,p2.z);
        glVertex3f(p6.x,p6.y,p6.z);
        glVertex3f(p4.x,p4.y,p4.z);

        glVertex3f(p1.x,p1.y,p1.z);
        glVertex3f(p3.x,p3.y,p3.z);
        glVertex3f(p7.x,p7.y,p7.z);
        glVertex3f(p5.x,p5.y,p5.z);
    glEnd();
}

void render_box(axis_aligned_box box)
{
    vector3d<float> p0=box.get_corner(0);
    vector3d<float> p1=box.get_corner(1);
    vector3d<float> p2=box.get_corner(2);
    vector3d<float> p3=box.get_corner(3);
    vector3d<float> p4=box.get_corner(4);
    vector3d<float> p5=box.get_corner(5);
    vector3d<float> p6=box.get_corner(6);
    vector3d<float> p7=box.get_corner(7);
    render_box(p0,p1,p2,p3,p4,p5,p6,p7);
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
                            std::size_t type)
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
                marching_cubes_render_algorithm::step(face_box,cube_num,model);
                render_box(face_box);
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
                                             std::shared_ptr<mesh_data> model)
{
    std::array<voxel,8> v = {n0->get_voxel(),n1->get_voxel(),
                             n4->get_voxel(),n5->get_voxel(),
                             n2->get_voxel(),n3->get_voxel(),
                             n6->get_voxel(),n7->get_voxel()};
    std::size_t cube_num=
    marching_cubes_render_algorithm::calculate_cube_num(v[0],v[1],v[2],v[3],
                                                        v[4],v[5],v[6],v[7]);
    //render_box(n0->get_offset(),n1->get_offset(),n2->get_offset(),n3->get_offset(),
    //           n4->get_offset(),n5->get_offset(),n6->get_offset(),n7->get_offset());

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
    auto new_center =n[min_size_num]->get_offset()-new_offset*2;
    /*std::cerr << n[min_size_num]->get_offset().x << "/"
              << n[min_size_num]->get_offset().y << "/"
              << n[min_size_num]->get_offset().z << std::endl;*/

    axis_aligned_box center_box(new_center,vector3d<std::size_t>(size,
                                                                 size,
                                                                 size));

    //std::cerr << new_center.x << "/" << new_center.y << "/" << new_center.z << std::endl;
    marching_cubes_render_algorithm::step(center_box,cube_num,model);
    render_box(center_box);
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
                // X-Axis Edge
                axis_aligned_box x_edge_box=center_box;
                x_edge_box.set_widths((float)min_size_x-size,size,size);
                x_edge_box.translate(min_size_x/2.0f,0,0);
                marching_cubes_render_algorithm::step(x_edge_box,cube_num,model);
                render_box(x_edge_box);
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
                // Y-Axis Edge
                axis_aligned_box y_edge_box=center_box;
                y_edge_box.set_widths(size,(float)min_size_y-size,size);
                y_edge_box.translate(0,min_size_y/2.0f,0);
                marching_cubes_render_algorithm::step(y_edge_box,cube_num,model);
                render_box(y_edge_box);
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
                // Z-Axis Edge
                if(min_size_x >= 4)
                    std::cerr << min_size_x << std::endl;
                axis_aligned_box z_edge_box=center_box;
                z_edge_box.set_widths(size,size,(float)min_size_z-size);
                z_edge_box.translate(0,0,min_size_z/2.0f);
                marching_cubes_render_algorithm::step(z_edge_box,cube_num,model);
                render_box(z_edge_box);
            }
        }

        // XY-Face
        cube_num=
    marching_cubes_render_algorithm::calculate_cube_num(v[3],v[3],v[3],v[3],
                                                        v[7],v[7],v[7],v[7]);
        if(min_size_x != 0 && min_size_y != 0) {
            process_dual_cell_face(n[3],n[7],cube_num,model,center_box,0x3);
        }

        // YZ-Face
        cube_num=
    marching_cubes_render_algorithm::calculate_cube_num(v[6],v[7],v[6],v[7],
                                                        v[6],v[7],v[6],v[7]);
        if(min_size_y != 0 && min_size_z != 0) {
            process_dual_cell_face(n[6],n[7],cube_num,model,center_box,0x6);
        }

        // XZ-Face
        cube_num=
    marching_cubes_render_algorithm::calculate_cube_num(v[5],v[5],v[7],v[7],
                                                        v[5],v[5],v[7],v[7]);
        if(min_size_x != 0 && min_size_z != 0) {
            process_dual_cell_face(n[5],n[7],cube_num,model,center_box,0x5);
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
                                            std::shared_ptr<mesh_data> model)
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
        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model);
    }else
    {
        create_dual_cells(n0,n1,n2,n3,n4,n5,n6,n7,model);
    }
}

void dual_marching_cubes_render_algorithm::edgeProcX( voxel_octree* n0,
                                                      voxel_octree* n1,
                                                      voxel_octree* n2,
                                                      voxel_octree* n3,
                                            std::shared_ptr<mesh_data> model)
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

        edgeProcX(c0,c2,c4,c6,model);
        edgeProcX(c1,c3,c5,c7,model);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model);
    }
}

void dual_marching_cubes_render_algorithm::edgeProcY( voxel_octree* n0,
                                                      voxel_octree* n1,
                                                      voxel_octree* n2,
                                                      voxel_octree* n3,
                                            std::shared_ptr<mesh_data> model)
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

        edgeProcY(c0,c1,c2,c3,model);
        edgeProcY(c4,c5,c6,c7,model);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model);
    }
}

void dual_marching_cubes_render_algorithm::edgeProcZ( voxel_octree* n0,
                                                      voxel_octree* n1,
                                                      voxel_octree* n2,
                                                      voxel_octree* n3,
                                            std::shared_ptr<mesh_data> model)
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

        edgeProcZ(c0,c1,c4,c5,model);
        edgeProcZ(c2,c3,c6,c7,model);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model);
    }
}

void dual_marching_cubes_render_algorithm::faceProcXY(voxel_octree* n0,
                                                      voxel_octree* n1,
                                            std::shared_ptr<mesh_data> model)
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

        faceProcXY(c0,c2,model);
        faceProcXY(c1,c3,model);
        faceProcXY(c4,c6,model);
        faceProcXY(c5,c7,model);

        edgeProcX(c0,c2,c4,c6,model);
        edgeProcX(c1,c3,c5,c7,model);

        edgeProcY(c0,c1,c2,c3,model);
        edgeProcY(c4,c5,c6,c7,model);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model);
    }
}

void dual_marching_cubes_render_algorithm::faceProcYZ(voxel_octree* n0,
                                                      voxel_octree* n1,
                                            std::shared_ptr<mesh_data> model)
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

        faceProcYZ(c0,c1,model);
        faceProcYZ(c2,c3,model);
        faceProcYZ(c4,c5,model);
        faceProcYZ(c6,c7,model);

        edgeProcY(c0,c1,c2,c3,model);
        edgeProcY(c4,c5,c6,c7,model);

        edgeProcZ(c0,c1,c4,c5,model);
        edgeProcZ(c2,c3,c6,c7,model);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model);
    }
}

void dual_marching_cubes_render_algorithm::faceProcXZ(voxel_octree* n0,
                                                      voxel_octree* n1,
                                            std::shared_ptr<mesh_data> model)
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

        faceProcXZ(c0,c4,model);
        faceProcXZ(c1,c5,model);
        faceProcXZ(c2,c6,model);
        faceProcXZ(c3,c7,model);

        edgeProcX(c0,c2,c4,c6,model);
        edgeProcX(c1,c3,c5,c7,model);

        edgeProcZ(c0,c1,c4,c5,model);
        edgeProcZ(c2,c3,c6,c7,model);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model);
    }
}

void dual_marching_cubes_render_algorithm::nodeProc(voxel_octree* n,
                                            std::shared_ptr<mesh_data> model)
{
    if(n->has_children())
    {
        // Visit all eight children
        for(int i=0; i<8; ++i)
            nodeProc(n->get_child(i),model);

        faceProcXY(n->get_child(0,0,0),n->get_child(0,0,1),model);
        faceProcXY(n->get_child(1,0,0),n->get_child(1,0,1),model);
        faceProcXY(n->get_child(0,1,0),n->get_child(0,1,1),model);
        faceProcXY(n->get_child(1,1,0),n->get_child(1,1,1),model);

        faceProcYZ(n->get_child(0,0,0),n->get_child(1,0,0),model);
        faceProcYZ(n->get_child(0,0,1),n->get_child(1,0,1),model);
        faceProcYZ(n->get_child(0,1,0),n->get_child(1,1,0),model);
        faceProcYZ(n->get_child(0,1,1),n->get_child(1,1,1),model);

        faceProcXZ(n->get_child(0,0,0),n->get_child(0,1,0),model);
        faceProcXZ(n->get_child(1,0,0),n->get_child(1,1,0),model);
        faceProcXZ(n->get_child(0,0,1),n->get_child(0,1,1),model);
        faceProcXZ(n->get_child(1,0,1),n->get_child(1,1,1),model);

        edgeProcX(n->get_child(0,0,0),n->get_child(0,0,1),
                  n->get_child(0,1,0),n->get_child(0,1,1),model);
        edgeProcX(n->get_child(1,0,0),n->get_child(1,0,1),
                  n->get_child(1,1,0),n->get_child(1,1,1),model);

        edgeProcY(n->get_child(0,0,0),n->get_child(1,0,0),
                  n->get_child(0,0,1),n->get_child(1,0,1),model);
        edgeProcY(n->get_child(0,1,0),n->get_child(1,1,0),
                  n->get_child(0,1,1),n->get_child(1,1,1),model);

        edgeProcZ(n->get_child(0,0,0),n->get_child(1,0,0),
                  n->get_child(0,1,0),n->get_child(1,1,0),model);
        edgeProcZ(n->get_child(0,0,1),n->get_child(1,0,1),
                  n->get_child(0,1,1),n->get_child(1,1,1),model);

        vertProc(n->get_child(0,0,0),n->get_child(1,0,0),
                 n->get_child(0,0,1),n->get_child(1,0,1),
                 n->get_child(0,1,0),n->get_child(1,1,0),
                 n->get_child(0,1,1),n->get_child(1,1,1),model);
    }
}

void render_octree(voxel_octree* octree)
{
    if(octree->has_children())
    {
        for(int i=0; i<8; ++i)
            render_octree(octree->get_child(i));
    }else
    {
        glColor3f(1,1,1);
        if(octree->get_voxel().is_opaque()&&
           octree->get_voxel().is_standard())
        {
            glColor3f(100,100,100);
        }else
            glColor3f(0,0,1);
            auto off=octree->get_offset();
            axis_aligned_box box(off,octree->get_size());
            render_box(box);
    }
}

void dual_marching_cubes_render_algorithm::process(voxel_model* node,graphics_service* service)
{
    std::cerr << "Rendering start" << std::endl;
    std::shared_ptr<mesh_data> model = std::make_shared<mesh_data>();

    voxel_data* r_d = node->get_render_data();
    voxel_octree* octree = voxel_octree::convert(r_d);
    nodeProc(octree,model);
    service->register_model((uintptr_t)node,model);
    std::cerr << "Rendering end" << std::endl;
    //render_octree(octree);
}

}
