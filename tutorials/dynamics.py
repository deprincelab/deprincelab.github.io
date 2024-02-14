 
import sys
import numpy as np

sys.path.insert(0, '/Users/deprince/software/psi4/install/lib/')

import psi4

if ( len(sys.argv) != 6 ):
    print ''
    print 'usage: python dynamics.py in(xyz) charge multiplicity out1(psi4) out2(xyz)'
    print ''
    sys.exit()

# read molecule from .xyz file
with open (str(sys.argv[1]), 'r') as myfile:
    mymol=myfile.read().splitlines()

# molecular charge
charge        = int(sys.argv[2])

# the multiplicity
multiplicity  = int(sys.argv[3])

# psi4 output file
psi4.set_output_file(str(sys.argv[4]),True)

# .xyz output file:
xyz_out       = open(str(sys.argv[5]),'w')

x     = []
y     = []
z     = []
label = []
mass  = []

# parse molecule
natom = int(mymol[0])
for i in range (natom):
    pieces = mymol[i+2].split()
    label.append(pieces[0])
    x.append(    pieces[1])
    y.append(    pieces[2])
    z.append(    pieces[3])
    mass.append( pieces[4])

# old x, y, z position
oldx = []
oldy = []
oldz = []

# gradient
dx = []
dy = []
dz = []

# new position
newx = []
newy = []
newz = []

for i in range (natom):
    oldx.append(x[i])
    oldy.append(y[i])
    oldz.append(z[i])

    newx.append(0.0)
    newy.append(0.0)
    newz.append(0.0)

    dx.append(0.0)
    dy.append(0.0)
    dz.append(0.0)


# start dynamics
total_time = 100.0
dt         = 0.2
t          = 0.0
while ( t < total_time ) :

    # molecule at current time, t
    mymol = ""
    for i in range (natom):
        mymol += label[i]
        mymol += " "
        mymol += x[i]
        mymol += " "
        mymol += y[i]
        mymol += " "
        mymol += z[i]
        mymol += "\n"

    # create psi4 molecule 
    mol = psi4.geometry( mymol )
    mol.set_molecular_charge(charge)
    mol.set_multiplicity(multiplicity)
    mol.reset_point_group('c1')
    mol.fix_orientation(True)
    mol.fix_com(True)
    mol.update_geometry()
    
    # set basis set
    psi4.set_options({'basis': 'sto-3g',
                      'df_basis_scf': 'sto-3g',
                      'scf_type' : 'df'})
    
    grad, wfn = psi4.gradient('scf',return_wfn=True)

    for i in range (natom):
        # gradient in hartree / bohr
        dx[i] = np.array(grad)[i][0]
        dy[i] = np.array(grad)[i][1]
        dz[i] = np.array(grad)[i][2]

        # new position in bohr
        newx[i] = 2.0 * float(x[i]) * 0.52917720859 - float(oldx[i]) * 0.52917720859 - 1.0 / float(mass[i]) * dx[i] * dt * dt
        newy[i] = 2.0 * float(y[i]) * 0.52917720859 - float(oldy[i]) * 0.52917720859 - 1.0 / float(mass[i]) * dy[i] * dt * dt
        newz[i] = 2.0 * float(z[i]) * 0.52917720859 - float(oldz[i]) * 0.52917720859 - 1.0 / float(mass[i]) * dz[i] * dt * dt

        # save old position
        oldx[i] = x[i]
        oldy[i] = y[i]
        oldz[i] = z[i]

        # new position in angstroms
        x[i] = str(newx[i] / 0.52917720859)
        y[i] = str(newy[i] / 0.52917720859)
        z[i] = str(newz[i] / 0.52917720859)

    xyz_out.write("%i\n" % natom)
    xyz_out.write("\n")
    for i in range (natom):
        xyz_out.write("%s %20.12f %20.12f %20.12f\n" % (mol.label(i),float(x[i]), float(y[i]), float(z[i])))
    xyz_out.flush()

    psi4.set_options({'guess': 'read'})

    t += dt

