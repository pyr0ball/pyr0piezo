// Pyr0-Piezo EasyPiezi Production Jig Alignment Fixture
// 3D printable fixture for consistent board placement
// Designed for fast insertion/removal during production runs
//
// Print settings:
// - Material: PETG or ABS (heat resistant)
// - Layer height: 0.2mm
// - Infill: 20-30%
// - Supports: Yes (for clamp mechanism)
// - Print time: ~2-3 hours

// ============================================================================
// CONFIGURATION - Adjust these to match your EasyPiezi board dimensions
// ============================================================================

// EasyPiezi board dimensions (measure your actual boards!)
board_length = 50;  // mm - adjust to actual
board_width = 30;   // mm - adjust to actual
board_thickness = 1.6;  // mm - standard PCB thickness

// Clearance and tolerances
board_clearance = 0.3;  // mm - extra space for easy insertion
alignment_pin_diameter = 3.0;  // mm - M3 alignment pins
alignment_pin_clearance = 0.2;  // mm

// Pogo pin contact area (where board must be pressed down)
pogo_contact_x = 15;  // mm from left edge - MEASURE YOUR JIG!
pogo_contact_y = 15;  // mm from bottom edge - MEASURE YOUR JIG!
pogo_contact_zone = 10;  // mm - size of contact area

// Fixture dimensions
base_thickness = 5;  // mm
wall_height = 15;  // mm - enough to guide board
wall_thickness = 3;  // mm

// Clamp mechanism
clamp_width = 15;  // mm
clamp_thickness = 3;  // mm
spring_recess_depth = 10;  // mm - for compression spring

// ============================================================================
// COMPUTED VALUES - Don't change these
// ============================================================================

inner_length = board_length + board_clearance * 2;
inner_width = board_width + board_clearance * 2;
outer_length = inner_length + wall_thickness * 2;
outer_width = inner_width + wall_thickness * 2;

// ============================================================================
// MAIN ASSEMBLY
// ============================================================================

module complete_fixture() {
    difference() {
        union() {
            // Base plate
            base_plate();

            // Alignment walls
            alignment_walls();

            // Alignment pins
            alignment_pin_holders();
        }

        // Board cavity
        board_cavity();

        // Alignment pin holes
        alignment_pin_holes();

        // Pogo pin access holes
        pogo_access_holes();
    }

    // Removable clamp (print separately)
    translate([outer_length + 10, 0, 0])
        spring_clamp();
}

// ============================================================================
// COMPONENTS
// ============================================================================

module base_plate() {
    color("lightblue")
    cube([outer_length, outer_width, base_thickness]);
}

module alignment_walls() {
    color("lightblue") {
        // Left wall
        translate([0, 0, base_thickness])
            cube([wall_thickness, outer_width, wall_height]);

        // Right wall
        translate([outer_length - wall_thickness, 0, base_thickness])
            cube([wall_thickness, outer_width, wall_height]);

        // Bottom wall
        translate([0, 0, base_thickness])
            cube([outer_length, wall_thickness, wall_height]);

        // Top wall (partial - leave opening for board insertion)
        translate([0, outer_width - wall_thickness, base_thickness])
            cube([outer_length / 2, wall_thickness, wall_height]);
    }
}

module alignment_pin_holders() {
    pin_holder_size = 8;

    color("lightblue") {
        // Left pin holder
        translate([wall_thickness + 5, wall_thickness + 5, 0])
            cylinder(d=pin_holder_size, h=base_thickness + wall_height);

        // Right pin holder
        translate([outer_length - wall_thickness - 5, outer_width - wall_thickness - 5, 0])
            cylinder(d=pin_holder_size, h=base_thickness + wall_height);
    }
}

module board_cavity() {
    color("red") {
        translate([wall_thickness, wall_thickness, base_thickness])
            cube([inner_length, inner_width, wall_height + 1]);
    }
}

module alignment_pin_holes() {
    pin_hole_diameter = alignment_pin_diameter + alignment_pin_clearance;

    color("red") {
        // Left pin hole (for M3 shoulder screw)
        translate([wall_thickness + 5, wall_thickness + 5, -1])
            cylinder(d=pin_hole_diameter, h=base_thickness + wall_height + 2);

        // Right pin hole
        translate([outer_length - wall_thickness - 5, outer_width - wall_thickness - 5, -1])
            cylinder(d=pin_hole_diameter, h=base_thickness + wall_height + 2);
    }
}

module pogo_access_holes() {
    // Cut holes in base for pogo pins to reach through
    color("red") {
        translate([wall_thickness + pogo_contact_x - pogo_contact_zone/2,
                   wall_thickness + pogo_contact_y - pogo_contact_zone/2,
                   -1])
            cube([pogo_contact_zone, pogo_contact_zone, base_thickness + 2]);
    }
}

module spring_clamp() {
    // Removable spring-loaded clamp piece
    color("orange") {
        difference() {
            union() {
                // Main clamp bar
                cube([outer_length, clamp_width, clamp_thickness]);

                // Spring pocket bosses
                translate([10, clamp_width/2, clamp_thickness])
                    cylinder(d=8, h=5);
                translate([outer_length - 10, clamp_width/2, clamp_thickness])
                    cylinder(d=8, h=5);
            }

            // Spring recesses
            translate([10, clamp_width/2, -1])
                cylinder(d=6, h=spring_recess_depth);
            translate([outer_length - 10, clamp_width/2, -1])
                cylinder(d=6, h=spring_recess_depth);

            // Alignment pin clearance holes
            translate([wall_thickness + 5, clamp_width/2, -1])
                cylinder(d=alignment_pin_diameter + 1, h=clamp_thickness + 10);
            translate([outer_length - wall_thickness - 5, clamp_width/2, -1])
                cylinder(d=alignment_pin_diameter + 1, h=clamp_thickness + 10);
        }
    }
}

// ============================================================================
// RENDER
// ============================================================================

// Render the complete fixture
complete_fixture();

// Add text labels for printing
translate([5, outer_width/2, base_thickness])
    linear_extrude(height=0.5)
        text("POGO", size=4, halign="left");

translate([outer_length + 10, clamp_width/2, 0])
    linear_extrude(height=0.5)
        text("CLAMP", size=4, halign="left");

// ============================================================================
// ASSEMBLY NOTES
// ============================================================================

/*
ASSEMBLY INSTRUCTIONS:

Hardware Required:
- 2x M3 x 16mm shoulder screws (alignment pins)
- 2x M3 nuts
- 2x compression springs (6mm OD x 10mm length)
- Optional: 4x rubber feet

Assembly Steps:
1. Print base fixture and clamp separately
2. Insert M3 shoulder screws through alignment pin holes from bottom
3. Secure with nuts on bottom (or thread into your jig base)
4. Place springs on spring bosses of clamp
5. Your programming jig should have matching holes for the alignment pins

Usage:
1. Place board on jig, align with pins
2. Press clamp down (springs compress)
3. Board makes contact with pogo pins
4. Flash firmware
5. Release clamp, remove board

ADJUSTMENT TIPS:
- If board is too loose: decrease board_clearance (line 16)
- If board is too tight: increase board_clearance
- Measure your actual board dimensions and update lines 13-15
- Measure your pogo pin positions and update lines 20-22
*/
