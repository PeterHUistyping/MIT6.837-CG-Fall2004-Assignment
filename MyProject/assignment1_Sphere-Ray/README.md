# assignment1
 debug: make sure *.C  clang++
 
 "-input", 
                "scene1_02.txt",   //input_file
                "-size", 
                "200", // width
                "200", // height
                "-output", 
                "output1_1.tga",  //output_file
                "-depth", 
                "9", //depth_min
                "10", //depth_max
                "depth1_1.tga"// depth_file


OrthographicCamera {  //camera //parser.getCamera()
    center 0 0 10 
    direction 0 0 -1
    up 0 1 0
    size 5
}

Background { color 0.1 0.1 0.1 }  //background_color  //parser.getBackgroundColor()

Materials {  
    numMaterials 3
    PhongMaterial {
	diffuseColor 1 0 0 
    }
    PhongMaterial {
	diffuseColor 0 1 0 
    }
    PhongMaterial {
	diffuseColor 0 0 1
    }
}

Group {  //parser.getGroup();
    numObjects 5//parser.getNumMaterials();
   
    MaterialIndex 0  //parser.getMaterial(i);
    Sphere {
	center 0 0 0 
        radius 1 
    }

    MaterialIndex 1
    Sphere {
	center 1 1 1 
        radius 0.75
    }
    Sphere {
	center -1 -1 1 
        radius 0.75
    }

    MaterialIndex 2
    Sphere {
	center -1 1 -1 
        radius 0.75
    }
    Sphere {
	center 1 -1 -1 
        radius 0.75 
    }
}




Scene description file grammar

file	::=	camera background materials group
camera	::=	orthographicCamera
orthographicCamera	::=	OrthographicCamera { 
    center Vec3f
    direction Vec3f
    up Vec3f
    size float 
}

background	::=	Background { 
    color Vec3f
}

materials	::=	Materials { 
    numMaterials int
    Material ^ numMaterials
}

Material	::=	Material { 
    diffuseColor Vec3f
}

group	::=	Group { 
    numObjects int
    (object3D | materialIndex object3D) ^ numObjects
}

object3D	::=	group | sphere
sphere	::=	Sphere { 
    center Vec3f
    radius float
}

materialIndex	::=	MaterialIndex int
Vec3f	::=	float float float

How to read a grammar:

Start with the top level rule, in this case file. To expand a rule, substitute the appropriate definitions for each of the elements in bold. Some special notation:
|	means "OR"
^ n	means "exactly n elements"