// A cube lamp that uses one 16x16 WS2812B panel driven by a Eternaglo Pico
// running RNDL firmware. The electrical hardware is USB C powered and sources
// the 5V supply and current for driving the LEDs.
//
// The lamp is composed of two pieces. The base and the shroud. The base
// holds the LED panel, electronics, and a mates to the shroud. The electronics
// sit beaneat the panel in a snap-fit cradle: the PCB's front edge seats in a
// groove in the front wall so the USB C receptacle passes through an oval
// port and sits flush with the exterior, side hooks snap over the PCB edges,
// and a back stop keeps cable insertion from pushing the board off the wall.
// The top of the frame is a narrow permiter pin that mates into the shroud's
// receiving slot.
//
// The shroud is printed as a vase to mate onto the frame such that the shroud's
// slot sits around the frame's pin permiter.
//    _                   _
//  _| |_  <-- Frame   |_| |_|   <-- shroud
//
//     +---------------+
//    /              / |
//   /              /  |
//  /              /   |
// +--------------+    |
// |              |    +
// |              |   //
// |              |  //
// |              | //
// |              |//
// +--------------+/
// +==============+/
//--------------------------------------------------------------------------
// Dimensions mm:
// WS2812B
//    Length: 160
//    Width : 160
//    Height:   8 (includes clearance for wiring)
//
// Eternaglo:
//    PCB:
//      Length: 36
//      Width:  17
//      Height:  4
//    Assembled:
//      USB-C from edge of PCB: 4
//      USB thickenest point:  15
//         ____________________
//      / |                    |
// Wire | |                    |]  <-- USB C extends from board edge
//      \ |____________________|
//--------------------------------------------------------------------------
//
// Configuration:
base_length = 160.0; // mm, nominal panel opening (length)
base_width = 160.0; // mm, nominal panel opening (width)

shroud_height = 160.0; // mm

// LED panel -----------------------------------------------------------------
panel_height = 8.0; // mm, includes clearance for wiring beneath the panel
panel_fit_clearance = 0.4; // mm, total clearance so the panel drops into the frame
panel_top_clearance = 0.5; // mm, small component clearance before the baffle rests on the panel

// Baffle grid: a 16x16 waffle insert that rests directly on the LED panel,
// isolating each LED's light so color-blend variance stays visible instead
// of washing into one tone, and doubling as a retainer that keeps the
// panel flat against the support pillars below it. Opaque filament is the
// point - it needs to BLOCK light sideways, unlike the translucent
// diffuser cap. Height is ~1:1 with the 10mm LED pitch for a ~45-degree
// cutoff; the remaining clearance is a short gap for final softening
// before the shroud.
baffle_height = 9.0; // mm, cell wall height
baffle_wall_thickness = 1.0; // mm, opaque divider thickness
baffle_fit_clearance = 0.4; // mm, total clearance so the grid drops into the panel cavity
baffle_top_clearance = 3.0; // mm, gap above the baffle before the rim/shroud

// Eternaglo Pico controller board --------------------------------------------
pcb_length = 36.0; // mm
pcb_width = 16.09; // mm, measured
pcb_thickness = 2.15; // mm, measured: board plus the USB-C connector's solder pads (sets the groove/snap height)
wire_gap_width = 8.0; // mm, centered gap in the back stop for LED panel wiring to exit
usb_extend = 2.0; // mm, USB-C connector overhang past the PCB edge
usb_assembled_height = 15.0; // mm, electronics cavity height above the floor (raised board + button must fit)

// USB-C receptacle: an oval (stadium) shell, mounted on the PCB's underside,
// so the oval port sits below the front-edge groove.
usb_c_width = 8.94; // mm, receptacle shell width (USB-C spec)
usb_c_height = 3.26; // mm, receptacle shell height (USB-C spec)
usb_c_below_pcb = 0.0; // mm, gap from PCB underside to shell top (0 = flush-mounted; negative for mid-mount)
usb_c_clearance = 0.3; // mm, total clearance around the shell in the oval port

// Snap-fit PCB cradle ----------------------------------------------------------
pcb_standoff = 5.5; // mm, PCB underside above the floor - must clear the receptacle hanging below it.
                    // Raised 1.5mm from the first test fit: the attached wires tilt the board.

// Tactile button on the PCB's top face near its front edge. It reaches into
// the front wall along with the board, so the groove gets a pocket above it.
button_size = 6.15; // mm, measured: square button footprint
button_height = 5.0; // mm, button height above the PCB top (6x6x5 tact switch - verify)
button_x = 0.0; // mm, button center offset from the board's centerline (+X = right, facing the port)
button_clearance = 0.4; // mm, total clearance around the button in the pocket
pcb_groove_clearance = 0.3; // mm, total vertical play in the front-wall groove
pcb_side_clearance = 0.2; // mm, per-side gap between PCB edges and the snap arms
pcb_back_clearance = 0.2; // mm, gap between PCB back edge and the back stop
pcb_support_width = 1.5; // mm, how far the support pads reach under the PCB edges
pcb_support_length = 4.0; // mm, Y length of each side support pad
snap_arm_thickness = 1.2; // mm, flexing arm wall (3 perimeters at 0.4mm)
snap_arm_length = 8.0; // mm, Y length of each snap arm
snap_lip = 0.6; // mm, how far the hook reaches in over the PCB edge
snap_lead_in = 1.6; // mm, height of the hook's lead-in ramp above the PCB
snap_vertical_play = 0.1; // mm, gap between PCB top and the hook's underside
snap_relief_depth = 1.6; // mm, floor pocket around each arm root - lengthens the flexing arm
snap_relief_gap = 1.0; // mm, pocket clearance around the arm
back_stop_thickness = 2.0; // mm, wall behind the PCB's back edge
back_stop_rise = 1.5; // mm, back stop height above the PCB top

// Base frame construction ----------------------------------------------------
wall_thickness = 3.2; // mm, frame side walls
floor_thickness = 2.4; // mm
corner_radius = 4.0; // mm, outer corner fillet
ledge_width = 2.0; // mm, inward step the panel rests on
ledge_thickness = 1.6; // mm, height of the flat panel-support shelf
ledge_chamfer = 2.0; // mm, height of the taper into the ledge (= ledge_width -> 45 degrees, self-supporting)
pin_height = 3.0; // mm, mating pin above the frame's top rim
pin_width = 1.4; // mm, radial width of the mating pin

// Panel support pillars: straight vertical posts from the floor up to the
// ledge height, so the 160mm panel span doesn't rely on the perimeter
// ledge alone. No overhangs, so they need no print supports of their own.
support_pillar_size = 6.0; // mm, square cross-section
support_pillar_spacing = 50.0; // mm, grid pitch, centered on the cavity
support_pillar_inset = 20.0; // mm, keep-out margin from the cavity walls
support_mount_clearance = 6.0; // mm, keep-out margin around the PCB mount

// Shroud construction ---------------------------------------------------------
shroud_wall_thickness = 1.6; // mm, single-wall vase shell (above the groove)
groove_wall_thickness = 1.0; // mm, thin retaining walls that form the groove
pin_fit_clearance = 0.2; // mm, per-side radial clearance between pin and groove
pin_seat_clearance = 0.4; // mm, vertical clearance so the shroud seats flush
groove_ceiling = 1.0; // mm, solid roof over the pocket connecting its inner
                      // wall to the outer wall (otherwise it's a detached ring)

// Mono-body shroud: the diffuser is the shroud's own solid top cap, not a
// separate part. Print this face down on the bed (flip the part before
// slicing) so the ~160mm top is fully supported by the build plate instead
// of bridging.
shroud_top_thickness = 1.6; // mm, top cap thickness - tune from a transmission test print
top_fillet_radius = 0.8; // mm, rounds the top-outer edge - the most visible edge of the print

// Which part to generate: base/shroud/baffle (print), a pcb_mount test
// coupon (just the front wall + PCB cradle, cut from the base, for dialing in
// the fit), or an assembled preview with translucent LED panel / PCB mock-ups.
part = "base"; // [base:Base (print), shroud:Shroud (print), baffle:Baffle grid (print), pcb_mount:PCB mount test (print), preview:Preview assembly]

$fn = 32;

//--------------------------------------------------------------------------
// Derived dimensions
//--------------------------------------------------------------------------
panel_opening_l = base_length + panel_fit_clearance;
panel_opening_w = base_width + panel_fit_clearance;

frame_outer_l = panel_opening_l + 2 * wall_thickness;
frame_outer_w = panel_opening_w + 2 * wall_thickness;

electronics_height = usb_assembled_height;
frame_rim_h = floor_thickness + electronics_height + ledge_thickness
            + panel_height + panel_top_clearance;
frame_total_h = frame_rim_h + pin_height;

// Baffle's seated height, in the base's own frame - directly on the panel.
baffle_z = floor_thickness + electronics_height + ledge_thickness
         + panel_height + panel_top_clearance;

// The mating pin sits inset from the frame's OUTER face by just enough to
// let the shroud's groove wall stay flush with the frame's outer surface
// (so the two parts read as one continuous wall once assembled), with the
// rest of its footprint free to land anywhere inside the wall thickness.
pin_outer_margin = groove_wall_thickness + pin_fit_clearance;
pin_outer_half_l = frame_outer_l / 2 - pin_outer_margin;
pin_outer_half_w = frame_outer_w / 2 - pin_outer_margin;

groove_pocket_depth = pin_height + pin_seat_clearance; // how far the pin actually inserts
groove_depth = groove_pocket_depth + groove_ceiling; // total height of the groove block

// PCB placement. The board's front edge sits in a groove cut into the front
// wall, deep enough that the USB-C overhang (usb_extend) ends exactly at the
// wall's outer face - the receptacle is flush with the exterior.
wall_inner_y = -panel_opening_w / 2;
pcb_groove_depth = max(wall_thickness - usb_extend, 0);
pcb_front_y = wall_inner_y - pcb_groove_depth;
pcb_back_y = pcb_front_y + pcb_length;
pcb_mid_y = (pcb_front_y + pcb_back_y) / 2;
pcb_half_w = pcb_width / 2;
pcb_z0 = floor_thickness + pcb_standoff; // PCB underside
pcb_z1 = pcb_z0 + pcb_thickness; // PCB top

// USB-C oval port, centered on the receptacle hanging below the board.
usb_port_w = usb_c_width + usb_c_clearance;
usb_port_h = usb_c_height + usb_c_clearance;
usb_port_z = pcb_z0 - usb_c_below_pcb - usb_c_height / 2;
assert(usb_port_z - usb_port_h / 2 >= floor_thickness,
       "pcb_standoff too small: the USB-C receptacle would hit the floor");

// Snap arm inner face, and the hook's underside / top heights.
snap_arm_x0 = pcb_half_w + pcb_side_clearance;
snap_hook_z0 = pcb_z1 + snap_vertical_play;
snap_hook_z1 = snap_hook_z0 + snap_lead_in;

back_stop_y0 = pcb_back_y + pcb_back_clearance;

// Mount footprint, for the support pillar grid to steer clear of.
mount_near_y = wall_inner_y;
mount_far_y = back_stop_y0 + back_stop_thickness;
mount_half_w = snap_arm_x0 + snap_arm_thickness + snap_relief_gap;

//--------------------------------------------------------------------------
// Helpers
//--------------------------------------------------------------------------

// 2D rectangle with filleted corners, centered at the origin.
module rrect(l, w, r) {
    r_clamped = min(r, min(l, w) / 2 - 0.01);
    if (r_clamped <= 0.05) {
        square([l, w], center = true);
    } else {
        hull() {
            for (x = [-1, 1], y = [-1, 1])
                translate([x * (l / 2 - r_clamped), y * (w / 2 - r_clamped)])
                    circle(r = r_clamped);
        }
    }
}

// 2D picture-frame ring: a rrect(l, w, r) outline of thickness t.
module rring(l, w, r, t) {
    difference() {
        rrect(l, w, r);
        rrect(l - 2 * t, w - 2 * t, r - t);
    }
}

// A rrect(l, w, r) slab of height h, with the TOP-outer edge rounded to a
// true radius (fillet_r) - the bottom stays sharp/flat so it butts flush
// against whatever it's sitting on. Standard minkowski-with-a-sphere trick:
// a thin inset (by fillet_r) disk swept by a sphere reproduces full size at
// the sphere's equator and tapers to the inset size at its pole, giving an
// exact quarter-circle profile; the sharp lower prism needs no trimming
// since the swept shape's lower half is always contained within it.
module rounded_top_slab(l, w, r, h, fillet_r) {
    disk_h = 0.01; // the minkowski "disk" has to have nonzero height; must
                    // be subtracted back out of its placement below, or the
                    // dome overshoots the slab's nominal top by this amount.
    union() {
        linear_extrude(height = h - fillet_r)
            rrect(l, w, r);
        translate([0, 0, h - fillet_r - disk_h])
            minkowski() {
                linear_extrude(height = disk_h)
                    offset(r = -fillet_r)
                        rrect(l, w, r);
                sphere(r = fillet_r, $fn = 24);
            }
    }
}

//--------------------------------------------------------------------------
// Base (frame)
//--------------------------------------------------------------------------

// Profile of the +X snap arm in the XZ plane: a straight arm rooted in the
// floor relief pocket, with a flat-bottomed hook that catches the PCB's top
// edge and a ramp above it that pushes the arm outward as the board is
// pressed down.
module snap_arm_profile() {
    x0 = snap_arm_x0;
    x1 = x0 + snap_arm_thickness;
    z_root = floor_thickness - snap_relief_depth;
    polygon([
        [x1, z_root],
        [x1, snap_hook_z1],
        [x0, snap_hook_z1],
        [x0 - snap_lip, snap_hook_z0],
        [x0, snap_hook_z0],
        [x0, z_root],
    ]);
}

// Snap-fit cradle for the Eternaglo Pico. The front edge seats in the front
// wall groove (see pcb_mount_cuts), two side arms snap over the PCB edges to
// hold it down, and a back stop behind the far edge keeps USB insertion from
// pushing the board (and its receptacle) away from the wall. Assemble by
// tipping the front edge into the groove, then pressing the back end down.
module pcb_mount() {
    for (side = [0, 1]) mirror([side, 0, 0]) {
        // Flexing snap arm, centered on the board's length.
        translate([0, pcb_mid_y, 0])
            rotate([90, 0, 0])
                linear_extrude(height = snap_arm_length, center = true)
                    snap_arm_profile();

        // Support pads under the PCB edge, either side of the arm (kept
        // clear of the arm's relief pocket so they don't stiffen it).
        for (dy = [-1, 1])
            translate([pcb_half_w - pcb_support_width / 2,
                       pcb_mid_y + dy * (snap_arm_length / 2 + snap_relief_gap + pcb_support_length / 2),
                       floor_thickness + pcb_standoff / 2])
                cube([pcb_support_width, pcb_support_length, pcb_standoff], center = true);
    }

    // Back stop, plus a support ledge under the PCB's back edge; both split
    // by the centered wire gap so the panel wiring can exit.
    stop_w = 2 * (pcb_half_w + pcb_side_clearance);
    difference() {
        union() {
            translate([-stop_w / 2, back_stop_y0, floor_thickness])
                cube([stop_w, back_stop_thickness, pcb_z1 + back_stop_rise - floor_thickness]);
            translate([-pcb_half_w, pcb_back_y - pcb_support_width, floor_thickness])
                cube([2 * pcb_half_w, pcb_support_width + pcb_back_clearance + 0.01, pcb_standoff]);
        }
        translate([-wire_gap_width / 2, pcb_back_y - pcb_support_width - 1, floor_thickness - 0.1])
            cube([wire_gap_width, pcb_support_width + pcb_back_clearance + back_stop_thickness + 2,
                  pcb_z1 + back_stop_rise]);
    }
}

// Material removed from the base for the PCB: the oval USB-C port through the
// front wall, the groove the PCB's front edge seats in, and the floor relief
// pockets that let the snap arms root lower (a longer arm flexes with less
// strain).
module pcb_mount_cuts() {
    // Oval (stadium) USB-C port through the full wall thickness.
    translate([0, -frame_outer_w / 2 - 0.5, usb_port_z])
        rotate([-90, 0, 0])
            linear_extrude(height = wall_thickness + 1)
                hull()
                    for (x = [-1, 1])
                        translate([x * (usb_port_w - usb_port_h) / 2, 0])
                            circle(d = usb_port_h);

    // Front-edge groove, centered on the board, cut back from the wall's inner face.
    if (pcb_groove_depth > 0)
        translate([-(pcb_half_w + pcb_side_clearance), pcb_front_y, pcb_z0 - pcb_groove_clearance / 2])
            cube([2 * (pcb_half_w + pcb_side_clearance), pcb_groove_depth + 0.1,
                  pcb_thickness + pcb_groove_clearance]);

    // Pocket above the groove so the button near the board's front edge
    // doesn't hit the wall's inner face.
    if (pcb_groove_depth > 0)
        translate([button_x - (button_size + button_clearance) / 2, pcb_front_y, pcb_z1])
            cube([button_size + button_clearance, pcb_groove_depth + 0.1,
                  button_height + button_clearance]);

    // Relief pockets around each snap arm's root.
    for (side = [-1, 1])
        translate([side * (snap_arm_x0 + snap_arm_thickness / 2) - (snap_arm_thickness / 2 + snap_relief_gap),
                   pcb_mid_y - snap_arm_length / 2 - snap_relief_gap / 2,
                   floor_thickness - snap_relief_depth])
            cube([snap_arm_thickness + 2 * snap_relief_gap, snap_arm_length + snap_relief_gap,
                  snap_relief_depth + 0.1]);
}

// Sparse grid of straight vertical posts from the floor to the ledge height,
// backing up the 160mm panel span against sag. Each post is a simple prism
// with no overhangs, so it prints without any support material of its own;
// the grid skips any point that would land inside the PCB mount footprint.
module support_pillars() {
    pillar_h = electronics_height + ledge_thickness;
    half_l = panel_opening_l / 2 - support_pillar_inset;
    half_w = panel_opening_w / 2 - support_pillar_inset;
    steps_l = floor(half_l / support_pillar_spacing);
    steps_w = floor(half_w / support_pillar_spacing);
    clear_x = mount_half_w + support_mount_clearance + support_pillar_size / 2;
    clear_y_min = mount_near_y - support_mount_clearance - support_pillar_size / 2;
    clear_y_max = mount_far_y + support_mount_clearance + support_pillar_size / 2;

    for (i = [-steps_l : steps_l])
        for (j = [-steps_w : steps_w]) {
            px = i * support_pillar_spacing;
            py = j * support_pillar_spacing;
            if (!(abs(px) < clear_x && py > clear_y_min && py < clear_y_max))
                translate([px, py, floor_thickness + pillar_h / 2])
                    cube([support_pillar_size, support_pillar_size, pillar_h], center = true);
        }
}

module base_frame() {
    difference() {
        union() {
            // Outer shell: floor + walls up to the top rim.
            linear_extrude(height = frame_rim_h)
                rrect(frame_outer_l, frame_outer_w, corner_radius);

            // Narrow mating pin, inset from the outer face (see pin_outer_margin).
            translate([0, 0, frame_rim_h])
                linear_extrude(height = pin_height)
                    rring(2 * pin_outer_half_l, 2 * pin_outer_half_w,
                          max(corner_radius - pin_outer_margin, 0.2), pin_width);
        }

        // Lower electronics cavity (full opening width), stopping short of
        // the ledge to leave room for the chamfer below it.
        translate([0, 0, floor_thickness])
            linear_extrude(height = electronics_height - ledge_chamfer)
                rrect(panel_opening_l, panel_opening_w, max(corner_radius - wall_thickness, 0.3));

        // Chamfer into the ledge at 45 degrees (ledge_chamfer == ledge_width)
        // instead of an abrupt step, so the shelf prints without an overhang.
        translate([0, 0, floor_thickness + electronics_height - ledge_chamfer])
            linear_extrude(height = ledge_chamfer,
                            scale = [(panel_opening_l - 2 * ledge_width) / panel_opening_l,
                                     (panel_opening_w - 2 * ledge_width) / panel_opening_w])
                rrect(panel_opening_l, panel_opening_w, max(corner_radius - wall_thickness, 0.3));

        // Ledge: a flat, constant-width shelf the panel actually rests on.
        translate([0, 0, floor_thickness + electronics_height])
            linear_extrude(height = ledge_thickness)
                rrect(panel_opening_l - 2 * ledge_width, panel_opening_w - 2 * ledge_width,
                      max(corner_radius - wall_thickness - ledge_width, 0.3));

        // Panel cavity (full opening width again) up through the top rim.
        translate([0, 0, floor_thickness + electronics_height + ledge_thickness])
            linear_extrude(height = frame_rim_h - (floor_thickness + electronics_height + ledge_thickness) + 0.1)
                rrect(panel_opening_l, panel_opening_w, max(corner_radius - wall_thickness, 0.3));

        pcb_mount_cuts();
    }

    pcb_mount();
    support_pillars();
}

// 16x16 waffle grid, one cell per LED (10mm pitch), printed separately in
// an opaque filament and dropped in to rest directly on the panel.
module baffle_grid() {
    cols = 16;
    rows = 16;
    pitch_l = base_length / cols;
    pitch_w = base_width / rows;
    outer_l = base_length - baffle_fit_clearance; // the perimeter grid lines are
    outer_w = base_width - baffle_fit_clearance;  // centered on the pitch lines and
                                                   // so overshoot the nominal footprint
                                                   // by half their thickness - clip it.

    linear_extrude(height = baffle_height)
        intersection() {
            union() {
                for (i = [0 : cols])
                    translate([-base_length / 2 + i * pitch_l - baffle_wall_thickness / 2, -base_width / 2])
                        square([baffle_wall_thickness, base_width]);
                for (j = [0 : rows])
                    translate([-base_length / 2, -base_width / 2 + j * pitch_w - baffle_wall_thickness / 2])
                        square([base_length, baffle_wall_thickness]);
            }
            square([outer_l, outer_w], center = true);
        }
}

//--------------------------------------------------------------------------
// Shroud
//--------------------------------------------------------------------------

module shroud() {
    // The groove is a solid block (outer wall + pin gap + inner wall, all
    // one connected piece) with a BLIND pocket cut from underneath to catch
    // the pin - open at the bottom for assembly, but with a solid ceiling
    // (groove_ceiling) left at the top so the inner wall isn't a separate
    // floating ring, detached from the rest of the shroud.
    a_l = frame_outer_l; // outer surface, flush with the frame
    a_w = frame_outer_w;
    pin_gap = pin_width + 2 * pin_fit_clearance;
    groove_wall_span = 2 * groove_wall_thickness + pin_gap; // full a-to-d thickness

    b_l = a_l - 2 * groove_wall_thickness; // pocket's outer edge
    b_w = a_w - 2 * groove_wall_thickness;

    r_a = corner_radius;
    r_b = max(r_a - groove_wall_thickness, 0.2);

    difference() {
        // Solid groove block - fully connected, no separate floating ring.
        linear_extrude(height = groove_depth)
            rring(a_l, a_w, r_a, groove_wall_span);

        // Blind pocket for the pin, cut from underneath only.
        translate([0, 0, -0.5])
            linear_extrude(height = groove_pocket_depth + 0.5)
                rring(b_l, b_w, r_b, pin_gap);
    }

    // Chamfer between the groove's thick ceiling (groove_wall_span) and the
    // thinner body wall above it. This part prints flipped - top cap down,
    // groove up - so the body prints first and the groove last; without
    // this taper the wall would jump straight from thin to thick going
    // into the groove, an overhang. Outer face stays flush; only the inner
    // face steps, same trick used for the base's ledge.
    groove_inner_l = a_l - 2 * groove_wall_span; // groove's own (narrow) inner opening
    groove_inner_w = a_w - 2 * groove_wall_span;
    groove_inner_r = max(r_a - groove_wall_span, 0.2);
    body_inner_l = a_l - 2 * shroud_wall_thickness; // body's (wide) inner opening
    body_inner_w = a_w - 2 * shroud_wall_thickness;
    body_inner_r = max(r_a - shroud_wall_thickness, 0.2);
    groove_chamfer_h = groove_wall_span - shroud_wall_thickness; // == run, for 45 degrees
    translate([0, 0, groove_depth])
        difference() {
            linear_extrude(height = groove_chamfer_h)
                rrect(a_l, a_w, r_a);
            linear_extrude(height = groove_chamfer_h,
                            scale = [body_inner_l / groove_inner_l, body_inner_w / groove_inner_w])
                rrect(groove_inner_l, groove_inner_w, groove_inner_r);
        }

    // Straight body above the chamfer - no taper, flush with the frame -
    // up to the solid top cap.
    body_h = shroud_height - groove_depth - groove_chamfer_h - shroud_top_thickness;
    translate([0, 0, groove_depth + groove_chamfer_h])
        linear_extrude(height = body_h)
            rring(a_l, a_w, r_a, shroud_wall_thickness);

    // Solid top cap - the diffuser itself, integrated into the same print.
    // Print this face down on the bed (flip before slicing) so this span
    // is fully supported instead of bridging. Its outer face (the top of
    // the assembled lamp - the most visible surface) gets a true rounded
    // edge instead of a sharp corner; the underside stays flush against
    // the body below.
    translate([0, 0, groove_depth + groove_chamfer_h + body_h])
        rounded_top_slab(a_l, a_w, r_a, shroud_top_thickness, top_fillet_radius);
}

//--------------------------------------------------------------------------
// Fit-check mock-ups (shown only in "preview")
//--------------------------------------------------------------------------

module panel_mock() {
    translate([0, 0, floor_thickness + electronics_height + ledge_thickness])
        linear_extrude(height = panel_height)
            square([base_length, base_width], center = true);
}

module pcb_mock() {
    translate([0, pcb_mid_y, pcb_z0 + pcb_thickness / 2])
        cube([pcb_width, pcb_length, pcb_thickness], center = true);
    // USB-C receptacle, its face flush with the frame's outer wall.
    translate([0, pcb_front_y - usb_extend, usb_port_z])
        rotate([-90, 0, 0])
            linear_extrude(height = usb_extend + 7)
                hull()
                    for (x = [-1, 1])
                        translate([x * (usb_c_width - usb_c_height) / 2, 0])
                            circle(d = usb_c_height);
}

//--------------------------------------------------------------------------
// Render
//--------------------------------------------------------------------------
if (part == "base") {
    base_frame();
} else if (part == "shroud") {
    // Print-ready orientation: flipped so the top cap (diffuser) sits on
    // the bed at z=0 and the groove opening faces up, fully supported
    // instead of bridging. A pure Z-axis mirror - X/Y untouched - not a
    // rotation. shroud() itself stays groove-down for assembly (used as-is
    // in the preview below).
    translate([0, 0, shroud_height])
        mirror([0, 0, 1])
            shroud();
} else if (part == "baffle") {
    baffle_grid();
} else if (part == "pcb_mount") {
    // Test coupon: just the front wall, floor and PCB cradle, cut out of the
    // full base so the fit can be dialed in without printing the whole frame.
    coupon_margin = 5.0;
    coupon_top = max(snap_hook_z1, pcb_z1 + back_stop_rise, pcb_z1 + button_height + button_clearance,
                     usb_port_z + usb_port_h / 2) + 3;
    intersection() {
        base_frame();
        translate([-(mount_half_w + coupon_margin), -frame_outer_w / 2 - 1, 0])
            cube([2 * (mount_half_w + coupon_margin),
                  mount_far_y + coupon_margin + frame_outer_w / 2 + 1, coupon_top]);
    }
} else {
    base_frame();
    translate([0, 0, frame_rim_h])
        shroud();
    %panel_mock();
    %pcb_mock();
    translate([0, 0, baffle_z])
        %baffle_grid();
}
