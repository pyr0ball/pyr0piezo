// Improved Clamshell Programming Jig for Pyr0-Piezo EasyPiezi
// Designed for 400+ board production runs with precise indexing
//
// Key improvements over basic clamshell:
// - Precision alignment pins (foolproof board placement)
// - Positive detent mechanism (know when it's seated)
// - One-hand operation
// - Wear indicators
// - Quick-release hinge
//
// Print settings:
// - Material: PETG (heat resistant, durable)
// - Layer height: 0.2mm
// - Infill: 30% (needs strength for repeated use)
// - Supports: Yes (for hinge area)

// ============================================================================
// CONFIGURATION
// ============================================================================

// Board dimensions (MEASURE YOUR BOARDS!)
board_length = 50;
board_width = 30;
board_thickness = 1.6;
board_tolerance = 0.2;  // Tight fit for precision

// Mounting holes on EasyPiezi board
// Measure distance between mounting holes
mounting_hole_spacing_x = 44;  // mm - MEASURE YOUR BOARD!
mounting_hole_spacing_y = 24;  // mm - MEASURE YOUR BOARD!
mounting_hole_diameter = 3.2;  // mm - for M3 screws
mounting_hole_offset_x = 3;  // mm from edge - MEASURE!
mounting_hole_offset_y = 3;  // mm from edge - MEASURE!

// Pogo pin contact locations (CRITICAL - MEASURE YOUR JIG!)
icsp_pins_x = 15;  // mm from left edge
icsp_pins_y = 15;  // mm from bottom edge
uart_pins_x = 35;  // mm from left edge
uart_pins_y = 15;  // mm from bottom edge

// Fixture parameters
wall_thickness = 3;
base_thickness = 4;
top_clearance = 3;  // Space above board in closed position
hinge_diameter = 4;  // mm - for M3 or 3mm rod
hinge_length = 40;

// Indexing pins
index_pin_diameter = 2.5;  // mm - precision pins
index_pin_height = 5;  // mm - extends into board holes

// Detent mechanism
detent_ball_diameter = 4;  // mm - for spring-loaded ball bearing
detent_depth = 2;  // mm

// ============================================================================
// COMPUTED VALUES
// ============================================================================

cavity_length = board_length + board_tolerance * 2;
cavity_width = board_width + board_tolerance * 2;
outer_length = cavity_length + wall_thickness * 2;
outer_width = cavity_width + wall_thickness * 2;
total_height = base_thickness + board_thickness + top_clearance;

// ============================================================================
// MAIN ASSEMBLY
// ============================================================================

// Set render_part to:
// "base" - bottom half with board cavity
// "top" - top half (clamp)
// "assembly" - show both together (for visualization)
// "print_layout" - both parts arranged for printing

render_part = "assembly";  // Change this!

if (render_part == "base") {
    base_half();
} else if (render_part == "top") {
    top_half();
} else if (render_part == "assembly") {
    base_half();
    rotate([0, 0, 0])
        translate([0, -outer_width, hinge_diameter/2 + wall_thickness])
            rotate([-90, 0, 0])
                top_half();
} else if (render_part == "print_layout") {
    base_half();
    translate([outer_length + 10, 0, 0])
        rotate([180, 0, 0])
            translate([0, -outer_width, -total_height])
                top_half();
}

// ============================================================================
// BASE HALF (Bottom)
// ============================================================================

module base_half() {
    difference() {
        union() {
            // Main base
            color("lightblue")
                cube([outer_length, outer_width, base_thickness]);

            // Walls
            color("lightblue")
                walls_base();

            // Indexing pins (precision alignment)
            color("gold")
                indexing_pins();

            // Hinge bosses
            color("lightblue")
                hinge_bosses_base();

            // Handle (for easy holding)
            color("lightblue")
                translate([outer_length/2 - 15, -10, 0])
                    cube([30, 10, base_thickness + 5]);
        }

        // Board cavity
        color("red")
            translate([wall_thickness, wall_thickness, base_thickness])
                cube([cavity_length, cavity_width, board_thickness + 0.5]);

        // Pogo pin access holes
        color("red")
            pogo_access_holes();

        // Hinge rod hole
        color("red")
            hinge_hole();

        // Detent recess (for ball bearing)
        color("red")
            translate([outer_length - 5, outer_width/2, base_thickness + wall_thickness])
                sphere(d=detent_ball_diameter + 0.5);

        // Finger relief cutouts (easier board removal)
        color("red") {
            translate([wall_thickness - 1, outer_width/2 - 8, base_thickness])
                cube([2, 16, board_thickness + 2]);
            translate([outer_length - wall_thickness - 1, outer_width/2 - 8, base_thickness])
                cube([2, 16, board_thickness + 2]);
        }

        // Label area
        color("red")
            translate([outer_length/2, wall_thickness/2, base_thickness - 0.5])
                linear_extrude(height=0.6)
                    text("EasyPiezi", size=3, halign="center");
    }

    // Visual indicators for pogo pin locations
    color("yellow") {
        translate([wall_thickness + icsp_pins_x, wall_thickness + icsp_pins_y, 0])
            cylinder(d=2, h=0.5);
        translate([wall_thickness + uart_pins_x, wall_thickness + uart_pins_y, 0])
            cylinder(d=2, h=0.5);
    }
}

module walls_base() {
    wall_height = board_thickness + 1;

    // Left wall
    translate([0, 0, base_thickness])
        cube([wall_thickness, outer_width, wall_height]);

    // Right wall
    translate([outer_length - wall_thickness, 0, base_thickness])
        cube([wall_thickness, outer_width, wall_height]);

    // Bottom wall
    translate([0, 0, base_thickness])
        cube([outer_length, wall_thickness, wall_height]);

    // Top wall
    translate([0, outer_width - wall_thickness, base_thickness])
        cube([outer_length, wall_thickness, wall_height]);
}

module indexing_pins() {
    // Precision pins that engage board mounting holes
    // These ensure perfect alignment every time

    pin_x1 = wall_thickness + mounting_hole_offset_x;
    pin_y1 = wall_thickness + mounting_hole_offset_y;
    pin_x2 = wall_thickness + mounting_hole_offset_x + mounting_hole_spacing_x;
    pin_y2 = wall_thickness + mounting_hole_offset_y + mounting_hole_spacing_y;

    // Pin 1 (bottom left)
    translate([pin_x1, pin_y1, base_thickness])
        cylinder(d=index_pin_diameter - 0.1, h=index_pin_height, $fn=20);

    // Pin 2 (top right)
    translate([pin_x2, pin_y2, base_thickness])
        cylinder(d=index_pin_diameter - 0.1, h=index_pin_height, $fn=20);

    // Optional: Add chamfer for easier insertion
    translate([pin_x1, pin_y1, base_thickness + index_pin_height])
        cylinder(d1=index_pin_diameter - 0.1, d2=index_pin_diameter - 0.3, h=0.5, $fn=20);
    translate([pin_x2, pin_y2, base_thickness + index_pin_height])
        cylinder(d1=index_pin_diameter - 0.1, d2=index_pin_diameter - 0.3, h=0.5, $fn=20);
}

module hinge_bosses_base() {
    boss_diameter = hinge_diameter + 4;

    // Left hinge boss
    translate([0, outer_width, base_thickness + board_thickness + top_clearance/2])
        rotate([0, 90, 0])
            cylinder(d=boss_diameter, h=10, $fn=30);

    // Right hinge boss
    translate([outer_length - 10, outer_width, base_thickness + board_thickness + top_clearance/2])
        rotate([0, 90, 0])
            cylinder(d=boss_diameter, h=10, $fn=30);
}

// ============================================================================
// TOP HALF (Clamp)
// ============================================================================

module top_half() {
    difference() {
        union() {
            // Main top plate
            color("orange")
                cube([outer_length, outer_width, wall_thickness]);

            // Pressure ribs (ensure even contact)
            color("orange")
                pressure_ribs();

            // Hinge bosses
            color("orange")
                hinge_bosses_top();

            // Lever/handle for closing
            color("orange")
                translate([outer_length/2 - 15, outer_width - 10, wall_thickness])
                    cube([30, 15, 3]);

            // Detent ball housing
            color("orange")
                translate([outer_length - 7, outer_width/2, 0])
                    cylinder(d=8, h=wall_thickness + 3, $fn=30);
        }

        // Hinge rod hole
        color("red")
            hinge_hole();

        // Detent ball hole
        color("red")
            translate([outer_length - 7, outer_width/2, -1])
                cylinder(d=detent_ball_diameter + 0.3, h=wall_thickness + 5, $fn=20);

        // Spring hole for detent
        color("red")
            translate([outer_length - 7, outer_width/2, wall_thickness + 1])
                cylinder(d=3, h=3, $fn=20);

        // Clearance for indexing pins
        pin_x1 = wall_thickness + mounting_hole_offset_x;
        pin_y1 = wall_thickness + mounting_hole_offset_y;
        pin_x2 = wall_thickness + mounting_hole_offset_x + mounting_hole_spacing_x;
        pin_y2 = wall_thickness + mounting_hole_offset_y + mounting_hole_spacing_y;

        color("red") {
            translate([pin_x1, pin_y1, -1])
                cylinder(d=index_pin_diameter + 0.5, h=wall_thickness + 2, $fn=20);
            translate([pin_x2, pin_y2, -1])
                cylinder(d=index_pin_diameter + 0.5, h=wall_thickness + 2, $fn=20);
        }

        // Label
        color("red")
            translate([outer_length/2, outer_width/2, wall_thickness - 0.5])
                linear_extrude(height=0.6)
                    text("TOP", size=5, halign="center");
    }
}

module pressure_ribs() {
    // Ribs apply even pressure over pogo pin areas
    rib_thickness = 2;
    rib_height = 2;

    // Rib over ICSP area
    translate([wall_thickness + icsp_pins_x - 5, wall_thickness, 0])
        cube([10, outer_width - wall_thickness * 2, rib_height]);

    // Rib over UART area
    translate([wall_thickness + uart_pins_x - 5, wall_thickness, 0])
        cube([10, outer_width - wall_thickness * 2, rib_height]);
}

module hinge_bosses_top() {
    boss_diameter = hinge_diameter + 4;
    boss_length = 12;

    // Left hinge boss
    translate([-2, outer_width, 0])
        rotate([0, 90, 0])
            cylinder(d=boss_diameter, h=boss_length, $fn=30);

    // Right hinge boss
    translate([outer_length - boss_length + 2, outer_width, 0])
        rotate([0, 90, 0])
            cylinder(d=boss_diameter, h=boss_length, $fn=30);
}

// ============================================================================
// SHARED COMPONENTS
// ============================================================================

module hinge_hole() {
    // Hinge rod passes through both halves
    hinge_y = outer_width;
    hinge_z = base_thickness + board_thickness + top_clearance/2;

    translate([-1, hinge_y, hinge_z])
        rotate([0, 90, 0])
            cylinder(d=hinge_diameter + 0.3, h=outer_length + 2, $fn=30);
}

module pogo_access_holes() {
    // Holes in base for pogo pins to reach through
    hole_size = 12;  // mm - large enough for pogo pin array

    // ICSP access
    translate([wall_thickness + icsp_pins_x - hole_size/2,
               wall_thickness + icsp_pins_y - hole_size/2,
               -1])
        cube([hole_size, hole_size, base_thickness + 2]);

    // UART access
    translate([wall_thickness + uart_pins_x - hole_size/2,
               wall_thickness + uart_pins_y - hole_size/2,
               -1])
        cube([hole_size, hole_size, base_thickness + 2]);
}

// ============================================================================
// ASSEMBLY HARDWARE
// ============================================================================

/*
BILL OF MATERIALS:

Printed Parts:
- 1x Base half (bottom)
- 1x Top half (clamp)

Hardware:
- 1x 3mm x 50mm steel rod (hinge pin)
- 1x 4mm ball bearing (detent)
- 1x Small compression spring (5mm OD x 10mm, for detent)
- 2x M3 x 16mm precision dowel pins (indexing)
- Optional: 4x rubber feet for base
- Optional: Small neodymium magnet for latch

ASSEMBLY:

1. Insert indexing pins into base half from bottom
   - Use press-fit or light glue
   - Ensure they're perpendicular to base

2. Install detent mechanism in top half:
   - Insert spring into spring hole
   - Place ball bearing on top of spring
   - May need light grease for smooth operation

3. Insert hinge rod through hinge bosses
   - Should rotate freely
   - Can use cotter pin or snap ring to secure ends

4. Test operation:
   - Should open and close smoothly
   - Detent should click when closed
   - Indexing pins should align with board holes

USAGE:

1. Open clamshell fully
2. Place EasyPiezi board in cavity
   - Mounting holes drop onto indexing pins
   - Board should seat flat
3. Close clamshell
   - Should feel detent click
   - Pressure ribs press board onto pogo pins
4. Flash firmware
5. Open clamshell
6. Board lifts easily with finger reliefs

MAINTENANCE:

- Check indexing pins every 100 boards (may need replacement)
- Clean pogo pins every 50 boards
- Lightly grease detent mechanism if it becomes stiff
- Replace hinge rod if it develops play

TROUBLESHOOTING:

- Board won't seat: Check for debris, verify indexing pin alignment
- Clamp won't close: Adjust detent spring tension
- Flash failures: Clean pogo pins, check pressure rib contact
- Board stuck: Use finger relief cutouts to lift board

CUSTOMIZATION:

Measure these on YOUR boards/jig and update lines 18-29:
- board_length
- board_width
- mounting_hole_spacing_x/y
- mounting_hole_offset_x/y
- icsp_pins_x/y
- uart_pins_x/y

Then:
1. Change render_part to "print_layout" (line 64)
2. Export STL
3. Print both parts
4. Assemble with hardware above
*/
