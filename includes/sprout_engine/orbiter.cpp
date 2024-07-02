//
// Created by Bellaedris on 18/06/2024.
//

#include "orbiter.h"
void Orbiter::lookat( const glm::vec3& center, const float size )
{
    m_focusPoint= center;
    m_position= glm::vec2(0, 0);
    m_rotation= glm::vec2(0, 180);
    m_distance = size;
}

void Orbiter::lookat(const glm::vec3& p_pmin, const glm::vec3& p_pmax )
{
    lookat((p_pmin + p_pmax) / 2.f, glm::distance(p_pmin, p_pmax));
}

void Orbiter::rotation( const float x, const float y )
{
    m_rotation.x= m_rotation.x + y;
    m_rotation.y= m_rotation.y + x;
}

void Orbiter::translation( const float x, const float y )
{
    m_position.x= m_position.x - m_distance * x;
    m_position.y= m_position.y + m_distance * y;
}

void Orbiter::move( const float z )
{
    m_distance = m_distance - m_distance * 0.01f * z;
    if(m_distance < 0.001f)
        m_distance= 0.001f;
}

glm::mat4 Orbiter::view( ) const
{
    glm::mat4 ret(1);
    ret = glm::translate(ret, {-m_position.x, -m_position.y, -m_distance});
    ret = glm::rotate(ret, glm::radians(m_rotation.x),{1, 0, 0});
    ret = glm::rotate(ret, glm::radians(m_rotation.y),{0, 1, 0});
    return glm::translate(ret, {-m_focusPoint.x, -m_focusPoint.y, -m_focusPoint.z});
}

glm::mat4 Orbiter::projection( const float p_aspectRatio, const float p_fov )
{
    m_aspectRatio = p_aspectRatio;
    m_fov= p_fov;

    return projection();
}

glm::mat4 Orbiter::projection( ) const
{
    // calcule la distance entre le centre de l'objet et la camera
    //~ Transform t= view();
    //~ Point c= t(m_center);
    //~ float d= -c.z;
    //~ float d= distance(m_center, Point(m_position.x, m_position.y, m_size));     // meme resultat plus rapide a calculer

    // regle near et far en fonction du centre et du rayon englobant l'objet
    //return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_zNear, m_zFar);
    float itan= 1 / tanf(glm::radians(m_fov) * 0.5f);
    float id= 1 / (m_zNear - m_zFar);

    return {
        itan / m_aspectRatio, 0, 0, 0,
        0, itan, 0, 0,
        0, 0, (m_zFar + m_zNear) * id, -1,
        0, 0, 2.f * m_zFar * m_zNear * id, 0
    };
}

glm::vec3 Orbiter::position( ) const
{
    glm::mat4 t= view();     // passage monde vers camera
    glm::mat4 tinv= glm::inverse(t);            // l'inverse, passage camera vers monde

    return  {(tinv * glm::vec4(0, 0, 0, 1))};        // la camera se trouve a l'origine, dans le repere camera...
}
