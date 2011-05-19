
# parallel netgen menus:  missing everything you cannot do anyway

menu .ngmenu -tearoff 0  -relief raised -bd 2
. configure -menu .ngmenu

.ngmenu add cascade -label "File" -menu .ngmenu.file -underline 0
.ngmenu add cascade -label "Geometry" -menu .ngmenu.geometry -underline 0
.ngmenu add cascade -label "Mesh" -menu .ngmenu.mesh -underline 0
.ngmenu add cascade -label "View" -menu .ngmenu.view -underline 0

if { $userlevel == 3} {
    .ngmenu add cascade -label "Special" -menu .ngmenu.special -underline 3
}

.ngmenu add cascade -label "Help" -menu .ngmenu.help -underline 0


#####################################################
#                                                   #
#     Menu File                                     #
#                                                   #
#####################################################

menu .ngmenu.file

# .ngmenu.file add separator

.ngmenu.file add command -label "Snapshot..." \
    -command { 
	set types { 
	    {"JPG file" {.jpg} } 
	    {"GIF file" {.gif} } 
	    {"PPM file" {.ppm} } 
	}
	set file [tk_getSaveFile -filetypes $types]
#  -defaultextension ".ppm"]
	if {$file != ""} {
	    .ndraw Ng_SnapShot $file }
    }



.ngmenu.file add command -label "Quit" -accelerator "<q>" \
    -command { puts "Thank you for using $progname"; Ng_Exit; destroy . }


#####################################################
#                                                   #
#     Menu Mesh                                     #
#                                                   #
#####################################################

menu .ngmenu.mesh


.ngmenu.mesh add command -label "Mesh Info" \
    -command {
	set dim [Ng_MeshInfo dim]
	set np [Ng_MeshInfo np]
	set ne [Ng_MeshInfo ne]
	set nse [Ng_MeshInfo nse]
	set nseg [Ng_MeshInfo nseg]
	set bbox [Ng_MeshInfo bbox]
	tk_messageBox -message  "Dimension: $dim\nPoints: $np\nElements: $ne\nSurface Els: $nse\nSegments: $nseg\nxmin [lindex $bbox 0] xmax [lindex $bbox 1]\nymin [lindex $bbox 2] ymax [lindex $bbox 3]\nzmin [lindex $bbox 4] zmax [lindex $bbox 5]"
    }


.ngmenu.mesh add command -label "Mesh Quality" \
    -command {
	set inplanemin 0
	set inplanemax 0
	set betplanemin 0
	set betplanemax 0
	Ng_MeshQuality inplanemin inplanemax betplanemin betplanemax
	puts "Triangle angles : $inplanemin - $inplanemax"
	puts "Tet angles      : $betplanemin - $betplanemax"
	tk_messageBox -message  "Triangle angles : $inplanemin - $inplanemax \n Tet angles      : $betplanemin - $betplanemax"
    }



.ngmenu.mesh add command -label "Analyze Geometry" \
    -command { Ng_GenerateMesh ag ag; Ng_ReadStatus; redraw }
.ngmenu.mesh add command -label "Mesh Edges" \
    -command { Ng_GenerateMesh me me; Ng_ReadStatus; redraw }
.ngmenu.mesh add command -label "Mesh Surface" \
    -command { set selectvisual mesh; Ng_SetVisParameters; \
		   Ng_GenerateMesh ms ms; Ng_ReadStatus; redraw }

.ngmenu.mesh add command -label "Optimize Surface" \
    -command { Ng_GenerateMesh os os cmsmSm; redraw }


#####################################################
#                                                   #
#     Menu Geometry                                 #
#                                                   #
#####################################################

menu .ngmenu.geometry
.ngmenu.geometry add command -label "Scan CSG Geometry" -command { Ng_ParseGeometry }
.ngmenu.geometry add command -label "CSG Options..." -command geometryoptionsdialog

.ngmenu.geometry add command -label "CSG Properties..." \
    -command topleveldialog2 

.ngmenu.geometry add separator

.ngmenu.geometry add command -label "STL Doctor..." \
    -command { stldoctordialog; }

.ngmenu.geometry add command -label "STL Info" \
    -command {
	set notriangles 0
	set minx 0
	set maxx 0
	set miny 0
	set maxy 0
	set minz 0
	set maxz 0
	set trigscons 0
	Ng_STLInfo notriangles minx maxx miny maxy minz maxz trigscons
	set msgtext "NO STL-Triangles : $notriangles\nGeometry:\nX = $minx - $maxx\nY = $miny - $maxy\nZ = $minz - $maxz\nConsistency Check = $trigscons\n"
	set msgtext "$msgtext Status: [Ng_STLInfo status]"
	tk_messageBox -title "STL Info" -message  $msgtext -type ok 
    }

.ngmenu.geometry add separator

.ngmenu.geometry add command -label "IGES/STEP Topology Explorer/Doctor..." \
    -command { occdialog; }


.ngmenu.geometry add command -label "OCC Construction" \
    -command { Ng_OCCConstruction; }




#####################################################
#                                                   #
#     Menu View                                     #
#                                                   #
#####################################################

menu .ngmenu.view
.ngmenu.view add command -label "Zoom all" \
    -command { Ng_ZoomAll; redraw }
.ngmenu.view add command -label "Center" \
    -command { Ng_Center; redraw }

.ngmenu.view add command -label "x-y plane" \
    -command { Ng_StandardRotation xy; redraw }
.ngmenu.view add command -label "y-x plane" \
    -command { Ng_StandardRotation yx; redraw }
.ngmenu.view add command -label "x-z plane" \
    -command { Ng_StandardRotation xz; redraw }
.ngmenu.view add command -label "z-x plane" \
    -command { Ng_StandardRotation zx; redraw }
.ngmenu.view add command -label "y-z plane" \
    -command { Ng_StandardRotation yz; redraw }
.ngmenu.view add command -label "z-y plane" \
    -command { Ng_StandardRotation zy; redraw }

.ngmenu.view add command -label "Viewing Options..." \
    -command { viewingoptionsdialog; redraw }
.ngmenu.view add command -label "Clipping Plane..." \
    -command { clippingdialog; redraw }
.ngmenu.view add command -label "Solution Data..." \
    -command { visual_dialog; redraw }
.ngmenu.view add checkbutton -variable viewqualityplot \
    -label "Quality Plot" \
    -command { qualityviewdialog $viewqualityplot }
.ngmenu.view add checkbutton -variable memuseplot \
    -label "Memory Usage" \
    -command { memusedialog $memuseplot }




# #####################################################
# #                                                   #
# #     Menu Refinement  -  not possible in parallel  #
# #                                                   #
# #####################################################




#####################################################
#                                                   #
#     Menu Special                                  #
#                                                   #
#####################################################

menu .ngmenu.special
.ngmenu.special add command -label "Insert virtual boundary layer" \
    -command { Ng_InsertVirtualBL; redraw }
.ngmenu.special add command -label "Cut off and combine with other" \
    -command { 
	set types { {"Mesh file"   {.vol}	} }
	set file [tk_getOpenFile -filetypes $types]
	if {$file != ""} {
	    Ng_CutOffAndCombine $file;  }
	redraw 
    }
.ngmenu.special add command -label "Helmholtz Mesh grading" \
    -command { Ng_HelmholtzMesh; }



# menu .mbar.stl.menu
# .mbar.stl.menu add command -label "STL options" \
#     -command { stloptionsdialog; }
#.mbar.stl.menu add command -label "STL Doctor" \
#    -command { stldoctordialog; }



#####################################################
#                                                   #
#     Menu Help                                     #
#                                                   #
#####################################################


menu .ngmenu.help
.ngmenu.help add command -label "Ng Help..." \
	-command { help_main }
# .ngmenu.view add checkbutton -variable showsensitivehelp \
#	-label "Sensitve Help" \
#	-command { sensitivehelpdialog $showsensitivehelp }
.ngmenu.view add checkbutton -label "Help Line" -variable showhelpline \
	-command {
    if { $showhelpline == 1} {
	pack .helpline -before .statbar -side bottom -fill x -padx 3p
    } {
	pack forget .helpline 
    }
} 

.ngmenu.help add command -label "About..." \
    -command {
tk_messageBox -message "This is NETGEN \n mainly written by \n Joachim Schöberl \n thanks to \n R. Gaisbauer, J. Gerstmayr"
}

# tk_menuBar .mbar .mbar.file .mbar.mesh .mbar.test .mbar.help
# focus .mbar





#####################################################
#                                                   #
#     Button bar                                    #
#                                                   #
#####################################################

frame .bubar -relief raised -bd 2
pack .bubar -side top -fill x

button .bubar.exitb -text "Quit" \
    -command { .ngmenu.file invoke "Quit" }
pack  .bubar.exitb -side left

button .bubar.zoomall -text "Zoom All" \
    -command { Ng_ZoomAll; redraw }

button .bubar.center -text "Center" \
    -command { Ng_Center; redraw }

# tk_optionMenu .bubar.modesel drawmode "rotate" "move  " "zoom  "
tixOptionMenu .bubar.modesel \
    -options {
	label.width  0
	label.anchor e
	menubutton.width 6
    } \
    -variable drawmode

.bubar.modesel add command rotate -label Rotate
.bubar.modesel add command move -label Move
.bubar.modesel add command zoom -label Zoom






set viewvals { geometry specpoints mesh solution}
if { $userlevel == 3} {
    set viewvals { geometry mesh specpoints surfmeshing modelview solution}
}

set viewvallabs(cross)     "Cross" 
set viewvallabs(geometry)  "Geometry" 
set viewvallabs(mesh)      "Mesh" 
set viewvallabs(specpoints) "Edges" 
set viewvallabs(surfmeshing) "Mesh Gen" 
set viewvallabs(modelview)     "Modeller" 
set viewvallabs(solution)     "Solution" 

tixOptionMenu .bubar.selview \
    -options {
	label.width  0
	label.anchor e
	menubutton.width 10
    } \

foreach viewv $viewvals {
    .bubar.selview add command $viewv -label $viewvallabs($viewv)
}

# geometry in parallel is not very interesting ;)
# set selectvisual mesh
Ng_SetSelectVisual
Ng_SetScalarFunction 
	
.bubar.selview config -variable selectvisual
.bubar.selview config -command { Ng_SetVisParameters; redraw }


pack .bubar.modesel -side right
pack forget .bubar.modesel
pack .bubar.center .bubar.zoomall .bubar.selview -side right

.ngmenu.view add checkbutton -variable viewrotatebutton \
    -label "Enable LeftButton Selection" \
    -command { 
	if { $viewrotatebutton } {
	    pack .bubar.modesel -side right
	} {
	    pack forget .bubar.modesel
	}
    }


#####################################################
#                                                   #
#     Status bar                                    #
#                                                   #
#####################################################

label .helpline -text "None"
pack forget .helpline -side bottom -fill x

frame .statbar -relief flat -bd 2
pack .statbar -side bottom -fill x

label .statbar.ptslabel -text "Points: "
label .statbar.ptsval -textvariable status_np
label .statbar.elslabel -text "   Elements: "
label .statbar.elsval -textvariable status_ne
label .statbar.selslabel -text "   Surf Elements: "
label .statbar.selsval -textvariable status_nse
label .statbar.memlabel -text "   Mem: "
label .statbar.memval -textvariable mem_moveable
label .statbar.task -textvariable status_task

pack .statbar.ptslabel .statbar.ptsval -side left -ipady 3p 
pack .statbar.elslabel .statbar.elsval -side left -ipady 3p 
pack .statbar.selslabel .statbar.selsval -side left -ipady 3p

if { $userlevel == 3 } {
    pack .statbar.memlabel .statbar.memval -side left -ipady 3p
}


tixMeter .statbar.per -value 0 -text 0%
.statbar.per configure -fillcolor blue

pack .statbar.per -side right
pack .statbar.task -side right -ipady 4


proc timer2 { } {
    global status_np
    global status_ne
    global status_nse
    global multithread_running
    global multithread_redraw
    global status_working
    global status_task
    global status_percent
    global status_tetqualclasses
    

    Ng_ReadStatus 

    if { $multithread_redraw == 1 } {
	set multithread_redraw 0;
	redraw;
    }

    global mem_moveable
    set mem_moveable [Ng_MemInfo moveable]


    .statbar.per config -value [expr $status_percent/100] -text [expr 0.1*int(10*$status_percent)]%


    if { $multithread_running } {
	pack .statbar.per -side right -before .statbar.task -padx 6
    } { 
	pack forget .statbar.per
    }
	

    
    # tet quality
    if {[winfo exists .qualityview_dlg] == 1} {
	
	global qualbar
	global qualbarnull
	global qualbaraxis

	set maxval 0
	for {set i 0} {$i < 20} {incr i} {
	    if {[lindex $status_tetqualclasses $i] > $maxval} {
		set maxval [lindex $status_tetqualclasses $i]
	    }
	} 

	set ubound 1
	while { $ubound < $maxval } {
	    set ubound [expr {10 * $ubound}]
	}
	if { $ubound/5 > $maxval } {
	    set ubound [expr $ubound/5]
	}
	if { $ubound/2 > $maxval } {
	    set ubound [expr $ubound/2]
	}


	
	for {set i 1} {$i <= 5} {incr i} {
	    global qualbaraxis($i)

	    set value [expr { $i * $ubound / 5 }]
	    .qualityview_dlg.c dchars $qualbaraxis($i) 0 end
	    .qualityview_dlg.c insert $qualbaraxis($i) end $value  
	}

	
	for {set i 0} {$i < 20} {incr i} {
	    set x1 [expr {100 + ($i*15) + 2}]
	    set x2 [expr {$x1+10}]
	    
	    set nbrs [lindex $status_tetqualclasses $i]
	    set y [expr (249 - (200 * $nbrs / $ubound ) )]
	    
	    global qualbar($i)
	    .qualityview_dlg.c coords $qualbar($i) $x1 250 $x2 $y

	    global qualbarnull($i)
	    if { $nbrs == 0 } {
		.qualityview_dlg.c itemconfigure $qualbarnull($i) -text 0
	    } {
		.qualityview_dlg.c itemconfigure $qualbarnull($i) -text "" 
	    }		
	}
	
    }



    if {[winfo exists .memuse_dlg] == 1} {    
	
	global memmark
	set usemb [Ng_MemInfo usedmb]
	for {set i 0} {$i < [string length $usemb] } {incr i} {
	    if { [string index $usemb $i] == 0 } {
		.memuse_dlg.c coords $memmark($i)  [expr 50+$i] 68 [expr 50+$i] 70
	    } {
		.memuse_dlg.c coords $memmark($i)  [expr 50+$i] 50 [expr 50+$i] 70
	    }
	}

    }





    after 200 { timer2 }
}
# after 1000 { timer2 }
timer2





proc bgerror { error } {
    global errorInfo userlevel
    if { $userlevel == 3} {
	puts "ERROR: $error" 
	puts "errinfo: $errorInfo"
    }
    tk_messageBox -title "Error Message" -message $error -type ok 
}






proc smh2 { menuitem } {
    if {[catch {$menuitem entrycget active -label} name]} {
	set name "    "
    } 
    show_menu_help $name 
    update idletasks
}

bind .ngmenu <<MenuSelect>> { smh2 %W }
bind .ngmenu.file <<MenuSelect>> { smh2 %W }
bind .ngmenu.geometry <<MenuSelect>> { smh2 %W }
bind .ngmenu.mesh <<MenuSelect>> { smh2 %W }
bind .ngmenu.view <<MenuSelect>> { smh2 %W }
bind .ngmenu.meshsize <<MenuSelect>> { smh2 %W }
bind .ngmenu.special <<MenuSelect>> { smh2 %W }
bind .ngmenu.help <<MenuSelect>> { smh2 %W }


# command bindings  
bind . <q> { .ngmenu.file invoke "Quit" }
bind . <l><g> { .ngmenu.file invoke "Load Geometry..." }  ; 
bind . <l><m> { .ngmenu.file invoke "Load Mesh..." }  ;
bind . <s><m> { .ngmenu.file invoke "Save Mesh..." }  ;
bind . <r><f> { .ngmenu.file activate "Recent Files" }  ;
bind . <n><p> { newprimitivedialog }      ; # 
bind . <e><p> { editprimitivedialog }
bind . <e><s> { newsoliddialog }
bind . <g><m> { .ngmenu.mesh invoke "Generate Mesh" }  ;

# bind . <v><m> { .bubar.selview configure activate  mesh }  ;




