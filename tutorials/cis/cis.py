import psi4
import numpy as np

# set molecule
mol = psi4.geometry("""
o
h 1 1.0
h 1 1.0 2 104.5
symmetry c1
""")  

# set options
psi4.set_options({'basis': 'sto-3g',
                  'scf_type': 'pk',
                  'e_convergence': 1e-8,
                  'd_convergence': 1e-8})

# compute the Hartree-Fock energy and wave function
scf_e, wfn = psi4.energy('SCF', return_wfn=True)

# Grab data from wavfunction

# number of doubly occupied orbitals
ndocc   = wfn.nalpha()

# total number of orbitals
nmo     = wfn.nmo()

# number of virtual orbitals
nvirt   = nmo - ndocc

# orbital energies
eps     = np.asarray(wfn.epsilon_a())

# occupied orbitals:
Co = wfn.Ca_subset("AO", "OCC")

# virtual orbitals:
Cv = wfn.Ca_subset("AO", "VIR")

# use Psi4's MintsHelper to generate ERIs
mints = psi4.core.MintsHelper(wfn.basisset())

# build the (ov|ov) integrals:
ovov = np.asarray(mints.mo_eri(Co, Cv, Co, Cv))

# build the (oo|vv) integrals:
oovv = np.asarray(mints.mo_eri(Co, Co, Cv, Cv))

# strip out occupied orbital energies, eps_o spans 0..ndocc-1
eps_o = eps[:ndocc]

# strip out virtual orbital energies, eps_v spans 0..nvirt-1
eps_v = eps[ndocc:]

# CIS Hamiltonian
Ham = np.zeros((ndocc*nvirt,ndocc*nvirt))

# build singlet hamiltonian
for i in range(0,ndocc):
    for a in range(0,nvirt):
        ia = i * nvirt + a
        for j in range(0,ndocc):
            for b in range(0,nvirt):
                jb = j * nvirt + b
                Ham[ia][jb] = 2.0 * ovov[i][a][j][b] - oovv[i][j][a][b]
        Ham[ia][ia] += eps_v[a] - eps_o[i]

# diagonalize Hamiltonian
eig = np.linalg.eigvals(Ham)

print("")
print("    ==> CIS singlet excitation energies (eV) <==")
print("")
for ia in range(0,ndocc*nvirt):
    print("    %5i %10.4f" % (ia,eig[ia]*27.21138))
print("")

# build triplet hamiltonian
for i in range(0,ndocc):
    for a in range(0,nvirt):
        ia = i * nvirt + a
        for j in range(0,ndocc):
            for b in range(0,nvirt):
                jb = j * nvirt + b
                Ham[ia][jb] = - oovv[i][j][a][b]
        Ham[ia][ia] += eps_v[a] - eps_o[i]

# diagonalize Hamiltonian
eig = np.linalg.eigvals(Ham)

print("")
print("    ==> CIS triplet excitation energies (eV) <==")
print("")
for ia in range(0,ndocc*nvirt):
    print("    %5i %10.4f" % (ia,eig[ia]*27.21138))
print("")
