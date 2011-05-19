
# userlevel 1..standard user 2..power-user 3..developer

set userlevel 3
if { [Ng_GetCommandLineParameter expert]=="defined" } {
    set userlevel 3
}

set progname "NETGEN"

set ngdir ""
if { [lsearch [array names env] NETGENDIR] != -1 } {
    set ngdir $env(NETGENDIR) 
}
if { [string length $ngdir] == 0 } {
    set ngdir "." 
}



set batchmode [Ng_GetCommandLineParameter batchmode]

if { $batchmode != "defined" } {
	catch {
     		wm withdraw .
     
     		wm title . $progname
     		wm geometry . =800x600
     		wm minsize . 400 300
	}
}


source ${ngdir}/ngtcltk/variables.tcl
source ${ngdir}/ngtcltk/parameters.tcl

catch { 
    source ${ngdir}/libsrc/parallel/menustat_parallel.tcl
}
catch { 
    source ${ngdir}/ngtcltk/dialog.tcl
}
catch {
    source ${ngdir}/ngtcltk/drawing.tcl
}

catch { 
    source ${ngdir}/ngtcltk/nghelp.tcl
}
catch { 
    source ${ngdir}/libsrc/parallel/ngvisual_parallel.tcl
}

catch {
    source ${ngdir}/ngtcltk/sockets.tcl
}

catch { source ${ngdir}/ngsolve/ngsolve_parallel.tcl }

# catch { [source ${ngdir}/ngsolve/preproc.tcl] }
# catch { [source ${ngdir}/ngsolve/pdecreator.tcl] }

# some applications:
#    source ${ngdir}/demoapp/demoapp.tcl
#    source ${ngdir}/metis/ngmetis.tcl



# trafo/zugstange : menu
##################################################
set zugstange 0


catch { source ${ngdir}/trafo/menu.tcl }




setgranularity ${meshoptions.fineness}

Ng_SetMeshingParameters
Ng_SetVisParameters
Ng_SetDebugParameters
Ng_STLDoctor
Ng_GeometryOptions set
Ng_SetOCCVisParameters

if { $batchmode != "defined" } {
	catch { 
     		wm protocol . WM_DELETE_WINDOW { .ngmenu.file invoke "Quit" }
     		wm deiconify .
	}
}

set trafoapp 0
catch { source ${ngdir}/trafoapp/trafoapp.tcl }

#####################
#
#  parallel - ignore command line parameters for processor 0
#
#####################


# set geofilename [Ng_GetCommandLineParameter geofile]
# if { $geofilename != "undefined" && 
#      [info exists trafo] == 0 && $zugstange == 0} {

#     if { [ catch { Ng_LoadGeometry $geofilename } errstring] == 0 } {
# 	AddRecentFile $geofilename
# 	Ng_ParseGeometry
# 	set selectvisual geometry
# 	Ng_SetVisParameters
# 	redraw
# 	wm title . [concat "$progname - " $geofilename]
# 	set dirname [file dirname $geofilename]
# 	set basefilename [file tail [file rootname $geofilename]]
#     } {
# 	puts "Problem with input file:"
# 	puts "$errstring"
#     }
# }


# set cnt 0
# foreach { gran } { verycoarse coarse moderate fine veryfine } {
#     set cnt [expr $cnt + 1]
#     if { [Ng_GetCommandLineParameter $gran] == "defined" } {
# 	set meshoptions.fineness $cnt
# 	setgranularity ${meshoptions.fineness}
#     }
# }


# set meshfilename [Ng_GetCommandLineParameter meshfile]
# if { $meshfilename == "undefined" } {
#     set meshfilename out.mesh
# }

# set meshfiletype [Ng_GetCommandLineParameter meshfiletype]
# if { $meshfiletype == "undefined" } {
#     set meshfiletype netgen
# }

# set inputmeshfilename [Ng_GetCommandLineParameter inputmeshfile]

# set mergemeshfilename [Ng_GetCommandLineParameter mergefile]

# set meshsizefilename [Ng_GetCommandLineParameter meshsizefile]

# if { $meshsizefilename != "undefined" } {
#     set options.meshsizefilename $meshsizefilename
# }


# if { $batchmode == "defined" } {

#     set options.parthread 0

# #    set selectvisual mesh
# #    Ng_SetVisParameters



#     if { $inputmeshfilename == "undefined" } {
# 	Ng_GenerateMesh ${meshoptions.firststep} ${meshoptions.laststep}
#     } else {
# 	Ng_LoadMesh $inputmeshfilename
# 	if { $mergemeshfilename != "undefined" } {
# 	    Ng_MergeMesh $mergemeshfilename
# 	}
#     }
	

#     if { $meshfiletype == "netgen" } {
# 	Ng_SaveMesh $meshfilename
#     } {
# 	if { [catch { Ng_ExportMesh $meshfilename $meshfiletype } ] == 1 } {
# 	    puts "Unknown file format $meshfiletype"
# 	}
#     }
    
#     exit
# }


# set stereo [Ng_GetCommandLineParameter stereo]
# if { $stereo == "defined" } {
#     set viewoptions.stereo 1 
#     puts "use stereo mode" 
#     Ng_SetVisParameters; 
#     redraw 
# }


# set scriptfilename [Ng_GetCommandLineParameter script]
# if { $scriptfilename != "undefined" } {
#     if { [catch { source $scriptfilename } errstring] == 1 } {
# 	puts "Error in input: $errstring"
#     }
# }


# if { [Ng_GetCommandLineParameter help]=="defined" } {
#     if { $zugstange == 1 } {
# 	print_zug_commandline_help
# 	exit;
#     } {
# 	if { $trafoapp == 1 } {
# 	    print_trafo_commandline_help;
# 	} {
# 	    print_commandline_help; 
# 	}
#     }
# }

# if { [file exists startup.tcl] } {
#     source startup.tcl }

# if { [Ng_GetCommandLineParameter recent]=="defined" } {
#     if { [catch { .ngmenu.solve invoke "Solve Recent PDE";  } errstring] == 1 } {
# 	puts "TCL-ERROR handler:\n $errstring";
# 	exit;
#     }
# }


#####
#
#   parallel - dont load pde file
#
#####

# set pdefilename [Ng_GetCommandLineParameter pdefile]
# if { $pdefilename != "undefined" } {
#     NGS_LoadPDE  $pdefilename;  

#     set solve [Ng_GetCommandLineParameter solve]
#     if { $zugstange == 1 } {
# 	set options.parthread 0
# 	NGS_SolvePDE;
#     } {
# 	if { $solve == "defined" } {
# 	    set options.parthread 0
# 	    NGS_SolvePDE
# 	    exit;
# 	} {
# 	    if { $solve != "undefined" } {
# 		set options.parthread 0
# 		for { set l 1 } { $l <= $solve } { incr l } { NGS_SolvePDE $l }
# 		exit;
# 	    }
# 	}
#     }
# }



# ##################################################
# catch { source ${ngdir}/trafo/trafo.tcl }


# # source ngusers/ebg/elasticity/ebgelast.tcl


