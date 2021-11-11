$fn = 200;
n = 6;


d = 391;
h = 5;
lip = 8;
th = 4; // thickness
dt = 30;

led_h = 12;


nut = 4;

module ring() {
    difference() {
        linear_extrude(dt) difference() {
            circle(d=d+2*th);
            circle(d=d-50);
        }
        translate([0,0,5]) linear_extrude(dt) circle(d=d-2*lip);
        translate([0,0,dt-h-th]) linear_extrude(h) circle(d=d);
        translate([0,0,5]) linear_extrude(led_h) circle(d=d-lip);
        a = 180/(n);
        #rotate([0,0,-a]) translate([d/2-15,0,0]) cylinder(d=nut, 5);
        #rotate([0,0,-a-a/2]) translate([d/2-15,0,0]) cylinder(d=nut, 5);
        #rotate([0,0,-a+a/2]) translate([d/2-15,0,0]) cylinder(d=nut, 5);
    }
}

difference() {
    ring();
    translate([-d,0,-1]) cube([d*2, d*2, dt*2]);
    rotate([0,0,180-360/(n)]) translate([-d,0,-1]) cube([d*2, d*2, dt*2]);
    rotate([0,0,-360/(n)]) translate([d/2-15,1,0]) rotate([0,0,-90]) cylinder(d=11, 5, $fn=3);
}

a = 180/(n);
translate([d/2-15,1,0]) rotate([0,0,-90]) cylinder(d=10, 5, $fn=3);