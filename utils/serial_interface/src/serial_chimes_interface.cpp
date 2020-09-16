/* Code author: Rebecca K. Lindsey (2020) */

#include<iostream>
#include<vector>
#include<string>
#include<cmath>


#include<fstream>

using namespace std;

#include "serial_chimes_interface.h"

// Simple linear algebra functions (for arbitrary triclinic cell support)

double mag_a    (const vector<double> & a)
{
    double mag = 0;
    
    for(int i=0; i<a.size(); i++)
        mag += a[i]*a[i];
    
    mag = sqrt(mag);
    
    return mag;
}
void   unit_a   (const vector<double> & a, vector<double> & unit)        
{
    double mag = mag_a(a);

    unit.resize(a.size());
    
    for(int i=0; i<a.size(); i++)
        unit[i] += a[i]/mag;
    
    return;
}
double a_dot_b  (const vector<double> & a, const vector<double> & b)
{
    double dot = 0;
    
    if (a.size() != b.size())
    {
        cout << "ERROR in a_dot_b: Vectors of different length!" << endl;
        exit(0);
    }
    
    for(int i=0; i<a.size(); i++)
        dot += a[i]*b[i];
    
    return dot;
}
double angle_ab (const vector<double> & a, const vector<double> & b)
{
    double ang = a_dot_b(a,b);

    ang /= mag_a(a);
    ang /= mag_a(b);

    return acos(ang);    
}        
void   a_cross_b(const vector<double> & a, const vector<double> & b, vector<double> & cross)
{
    if( a.size() != b.size())
    {
        cout << "ERROR in a_cross_b: Vectors of different length!" << endl;
        exit(0);
    }
    if(a.size() != 3)
    {
        cout << "ERROR in a_cross_b: Vectors should be of length 3!" << endl;
        exit(0);
    }
    
    cross.resize(3);
    cross[0] =    (a[1]*b[2] - a[2]*b[1]);
    cross[1] = -1*(a[0]*b[2] - a[2]*b[0]);
    cross[2] =    (a[0]*b[1] - a[1]*b[0]);
    return;
}    
    
// serial_chimes_interface member functions

serial_chimes_interface::serial_chimes_interface()
{
    // Initialize Pointers, etc for chimes calculator interfacing (2-body only for now)
    // To set up for many body calculations, see the LAMMPS implementation

    hmat     .resize(9);
    invr_hmat.resize(9);

    dist_3b.resize(3);
    dist_4b.resize(4);
    
    dr   .resize(3);
    dr_3b.resize(3,std::vector<double>(3));  
    dr_4b.resize(6,std::vector<double>(3));
    
    force_ptr_2b.resize(2,std::vector<double*>(3));
    force_ptr_3b.resize(3,std::vector<double*>(3));
    force_ptr_4b.resize(4,std::vector<double*>(3));
    
    typ_idxs_2b.resize(2);
    typ_idxs_3b.resize(3);
    typ_idxs_4b.resize(4);
    
    max_2b_cut = 0.0;
    max_3b_cut = 0.0;
    max_4b_cut = 0.0;
    
}
serial_chimes_interface::~serial_chimes_interface()
{}
void serial_chimes_interface::init_chimesFF(string chimesFF_paramfile, int layers)
{
    n_layers = layers;
    
    // Initialize the chimesFF object, read parameters
    
    init(0);
    read_parameters(chimesFF_paramfile);
    set_atomtypes(type_list);
}
void serial_chimes_interface::reorient_system(vector<double> & x_in, vector<double> & y_in, vector<double> & z_in, vector<double> & cella_in, vector<double> & cellb_in, vector<double> & cellc_in, vector<string> & atmtyps)
{
    // Note: This function assumes the system is always arbitrarily oriented and triclinic, and atoms are not wrapped. 
    //       The result is slow but general and readable.
    
    
    //////////////////////////////////////////
    // STEP 1: Copy the system
    //////////////////////////////////////////
    
    n_atoms = x_in.size();
    
    // Sanity checks
    
    if (n_atoms != y_in.size())
    {
        cout << "ERROR: x and y coordinate vector lengths do not match!" << endl;
        exit(0);
    }
    if (n_atoms != z_in.size())
    {
        cout << "ERROR: x and z coordinate vector lengths do not match!" << endl;
        exit(0);
    }
    
    // Copy over the system
    
    n_ghost = n_atoms;
    
    for (int a=0; a<n_atoms; a++)
    {
        sys_atmtyps.push_back(atmtyps[a]);    

        sys_x.push_back( x_in[a] );
        sys_y.push_back( y_in[a] );
        sys_z.push_back( z_in[a] );
        
        sys_parent.push_back(a);
    }    

    // Define the h-matrix (stores the cell vectors locally)

    hmat[0] = cella_in[0];  hmat[3] = cella_in[1]; hmat[6] = cella_in[2];
    hmat[1] = cellb_in[0];  hmat[4] = cellb_in[1]; hmat[7] = cellb_in[2];
    hmat[2] = cellc_in[0];  hmat[5] = cellc_in[1]; hmat[8] = cellc_in[2];

    // Determine the h-matrix inverse
        
    double hmat_det = hmat[0] * (hmat[4]*hmat[8] - hmat[5]*hmat[7])
                    - hmat[1] * (hmat[3]*hmat[8] - hmat[5]*hmat[6])
                    + hmat[2] * (hmat[3]*hmat[7] - hmat[4]*hmat[6]);
    
    vector<double> tmp_vec(9);
        
    tmp_vec[0] =      (hmat[4]*hmat[8] - hmat[5]*hmat[7]); tmp_vec[3] = -1 * (hmat[1]*hmat[8] - hmat[2]*hmat[7]); tmp_vec[6] =      (hmat[1]*hmat[5] - hmat[2]*hmat[4]);
    tmp_vec[1] = -1 * (hmat[3]*hmat[8] - hmat[5]*hmat[6]); tmp_vec[4] =      (hmat[0]*hmat[8] - hmat[2]*hmat[6]); tmp_vec[7] = -1 * (hmat[0]*hmat[5] - hmat[2]*hmat[3]);
    tmp_vec[2] =      (hmat[3]*hmat[7] - hmat[4]*hmat[6]); tmp_vec[5] = -1 * (hmat[0]*hmat[7] - hmat[1]*hmat[6]); tmp_vec[8] =      (hmat[0]*hmat[4] - hmat[1]*hmat[3]);
        
    invr_hmat[0] = tmp_vec[0]; invr_hmat[3] = tmp_vec[1]; invr_hmat[6] = tmp_vec[2];
    invr_hmat[1] = tmp_vec[3]; invr_hmat[4] = tmp_vec[4]; invr_hmat[7] = tmp_vec[5];
    invr_hmat[2] = tmp_vec[6]; invr_hmat[5] = tmp_vec[7]; invr_hmat[8] = tmp_vec[8];

    invr_hmat[0] /= hmat_det; invr_hmat[3] /= hmat_det; invr_hmat[6] /= hmat_det;
    invr_hmat[1] /= hmat_det; invr_hmat[4] /= hmat_det; invr_hmat[7] /= hmat_det;
    invr_hmat[2] /= hmat_det; invr_hmat[5] /= hmat_det; invr_hmat[8] /= hmat_det;


    //////////////////////////////////////////
    // STEP 2: Wrap atoms
    //////////////////////////////////////////
        
    // Wrap atoms by converting to inverse space (orthorhombic, cell lengths = unity)
    // Leave in fractional coordinates since we'll need to transform to the new basis later
        
    double tmp_ax, tmp_ay, tmp_az;

    for(int i=0; i<n_atoms; i++)
    {
        tmp_ax = invr_hmat[0]*sys_x[i] + invr_hmat[1]*sys_y[i] + invr_hmat[2]*sys_z[i];
        tmp_ay = invr_hmat[3]*sys_x[i] + invr_hmat[4]*sys_y[i] + invr_hmat[5]*sys_z[i];
        tmp_az = invr_hmat[6]*sys_x[i] + invr_hmat[7]*sys_y[i] + invr_hmat[8]*sys_z[i];

        sys_x[i] = tmp_ax - floor(tmp_ax);
        sys_y[i] = tmp_ay - floor(tmp_ay);
        sys_z[i] = tmp_az - floor(tmp_az);
    }

    //////////////////////////////////////////
    // STEP 3: Manipulate atoms and cell to conform with LAMMPS convention
    //////////////////////////////////////////
    // This will make linear-scaling neighbor list construction easier down the line
    // See: https://lammps.sandia.gov/doc/Howto_triclinic.html
    
    // Rotate the cell
    
    vector<double> tmp_cella(3);
    vector<double> tmp_cellb(3);
    vector<double> tmp_cellc(3);
    vector<double> tmp_unit (3);
    vector<double> tmp_cross(3);
    
    unit_a   (cella_in, tmp_unit);
    a_cross_b(tmp_unit, cellb_in, tmp_cross);
    
    // Determine cell vectors for the rotated system
    
    tmp_cella[0] = mag_a(cella_in);
    tmp_cella[1] = 0;
    tmp_cella[2] = 0;

    tmp_cellb[0] = a_dot_b(cellb_in, tmp_unit);
    tmp_cellb[1] = mag_a(tmp_cross);
    tmp_cellb[2] = 0;

    tmp_cellc[0] = a_dot_b(cellc_in,tmp_unit);
    tmp_cellc[1] = (a_dot_b(cellb_in,cellc_in) - tmp_cellb[0]*tmp_cellc[0]) / tmp_cellb[1];
    tmp_cellc[2] = sqrt( mag_a(cellc_in)*mag_a(cellc_in) -  tmp_cellc[0]*tmp_cellc[0] - tmp_cellc[1]*tmp_cellc[1]);
    
            
    // Determine the new cell h-matrix and its inverse

    hmat[0] = tmp_cella[0];  hmat[3] = tmp_cella[1]; hmat[6] = tmp_cella[2];
    hmat[1] = tmp_cellb[0];  hmat[4] = tmp_cellb[1]; hmat[7] = tmp_cellb[2];
    hmat[2] = tmp_cellc[0];  hmat[5] = tmp_cellc[1]; hmat[8] = tmp_cellc[2];

    hmat_det = hmat[0] * (hmat[4]*hmat[8] - hmat[5]*hmat[7])
             - hmat[1] * (hmat[3]*hmat[8] - hmat[5]*hmat[6])
             + hmat[2] * (hmat[3]*hmat[7] - hmat[4]*hmat[6]);

    tmp_vec[0] =      (hmat[4]*hmat[8] - hmat[5]*hmat[7]); tmp_vec[3] = -1 * (hmat[1]*hmat[8] - hmat[2]*hmat[7]); tmp_vec[6] =      (hmat[1]*hmat[5] - hmat[2]*hmat[4]);
    tmp_vec[1] = -1 * (hmat[3]*hmat[8] - hmat[5]*hmat[6]); tmp_vec[4] =      (hmat[0]*hmat[8] - hmat[2]*hmat[6]); tmp_vec[7] = -1 * (hmat[0]*hmat[5] - hmat[2]*hmat[3]);
    tmp_vec[2] =      (hmat[3]*hmat[7] - hmat[4]*hmat[6]); tmp_vec[5] = -1 * (hmat[0]*hmat[7] - hmat[1]*hmat[6]); tmp_vec[8] =      (hmat[0]*hmat[4] - hmat[1]*hmat[3]);
        
    invr_hmat[0] = tmp_vec[0]; invr_hmat[3] = tmp_vec[1]; invr_hmat[6] = tmp_vec[2];
    invr_hmat[1] = tmp_vec[3]; invr_hmat[4] = tmp_vec[4]; invr_hmat[7] = tmp_vec[5];
    invr_hmat[2] = tmp_vec[6]; invr_hmat[5] = tmp_vec[7]; invr_hmat[8] = tmp_vec[8];

    invr_hmat[0] /= hmat_det; invr_hmat[3] /= hmat_det; invr_hmat[6] /= hmat_det;
    invr_hmat[1] /= hmat_det; invr_hmat[4] /= hmat_det; invr_hmat[7] /= hmat_det;
    invr_hmat[2] /= hmat_det; invr_hmat[5] /= hmat_det; invr_hmat[8] /= hmat_det;      
    

    // Transform to the new nominally rotated cell  

    for(int i=0; i<n_atoms; i++)
    {   
        tmp_ax = sys_x[i];
        tmp_ay = sys_y[i];
        tmp_az = sys_z[i];
            
        sys_x[i] = hmat[0]*tmp_ax + hmat[1]*tmp_ay + hmat[2]*tmp_az;	
        sys_y[i] = hmat[3]*tmp_ax + hmat[4]*tmp_ay + hmat[5]*tmp_az;
        sys_z[i] = hmat[6]*tmp_ax + hmat[7]*tmp_ay + hmat[8]*tmp_az;
	
        tmp_ax = invr_hmat[0]*sys_x[i] + invr_hmat[1]*sys_y[i] + invr_hmat[2]*sys_z[i];
        tmp_ay = invr_hmat[3]*sys_x[i] + invr_hmat[4]*sys_y[i] + invr_hmat[5]*sys_z[i];
        tmp_az = invr_hmat[6]*sys_x[i] + invr_hmat[7]*sys_y[i] + invr_hmat[8]*sys_z[i];
	

        tmp_ax -= floor(tmp_ax);
        tmp_ay -= floor(tmp_ay);
        tmp_az -= floor(tmp_az);	
        sys_x[i] = hmat[0]*tmp_ax + hmat[1]*tmp_ay + hmat[2]*tmp_az;	
        sys_y[i] = hmat[3]*tmp_ax + hmat[4]*tmp_ay + hmat[5]*tmp_az;
        sys_z[i] = hmat[6]*tmp_ax + hmat[7]*tmp_ay + hmat[8]*tmp_az;

    }   

	//////////////////////////////////////////
	// STEP 3: Determine cell extents and volume for neighbor list constructions
	//////////////////////////////////////////    

	double latcon_a = mag_a({hmat[0], hmat[3], hmat[6]});
	double latcon_b = mag_a({hmat[1], hmat[4], hmat[7]});
	double latcon_c = mag_a({hmat[2], hmat[5], hmat[8]});

	double cell_alpha = angle_ab({hmat[1], hmat[4], hmat[7]}, {hmat[2], hmat[5], hmat[8]});
	double cell_beta  = angle_ab({hmat[2], hmat[5], hmat[8]}, {hmat[0], hmat[3], hmat[6]});
	double cell_gamma = angle_ab({hmat[0], hmat[3], hmat[6]}, {hmat[1], hmat[4], hmat[7]});

	double cell_lx = latcon_a;
	double xy = latcon_b * cos(cell_gamma);
	if(xy < 1E-12)
	    xy = 0.0;

	double xz = latcon_c * cos(cell_beta );
	if(xz < 1E-12)
	    xz = 0.0;

	double cell_ly = sqrt(latcon_b *latcon_b - xy*xy);

	double yz = (latcon_b*latcon_c * cos(cell_alpha) -xy*xz)/cell_ly;
	if(yz < 1E-12)
	    yz = 0.0;

	double cell_lz = sqrt( latcon_c*latcon_c - xz*xz -yz*yz);

	double tmp = xy;

	if(xz > tmp)
	    tmp = xz;
	if((xy+xz) > tmp)
	    tmp = xy+xz;

	extent_x = cell_lx + tmp; 
	extent_y = cell_ly + yz;
	extent_z = cell_lz;        

	vol  = 1;
	vol += 2*cos(cell_alpha)*cos(cell_beta )*cos(cell_gamma);
	vol -=   cos(cell_alpha)*cos(cell_alpha);
	vol -=   cos(cell_beta )*cos(cell_beta );
	vol -=   cos(cell_gamma)*cos(cell_gamma);

	vol = latcon_a * latcon_b * latcon_c * sqrt(vol);        

}
void serial_chimes_interface::calculate(vector<double> & x_in, vector<double> & y_in, vector<double> & z_in, vector<double> & cella_in, vector<double> & cellb_in, vector<double> & cellc_in, vector<string> & atmtyps, double & energy, vector<vector<double> > & force, vector<double> & stress)
{
    reorient_system(x_in, y_in, z_in, cella_in, cellb_in, cellc_in, atmtyps);
    build_layered_system(atmtyps);
    build_neigh_lists();
    set_atomtyp_indices();
    
    // Setup vars
    
    int ii, jj, kk, ll;
    
    vector<double*> stensor(9);

    for (int idx=0; idx<9; idx++)
        stensor[idx]  = &stress[idx];
    
    
    ////////////////////////
    // interate over 1- and 2b's 
    ////////////////////////

    for(int i=0; i<n_atoms; i++)
    {
        compute_1B(sys_atmtyp_indices[i], energy);

        for(int j=0; j<neighlist_2b[i].size(); j++) // Neighbors of i
        {
            jj = neighlist_2b[i][j];
            
            dist = get_dist(i,jj,dr); // Populates dr, which is passed by ref (overloaded)
            
            typ_idxs_2b[0] = sys_atmtyp_indices[i ];
            typ_idxs_2b[1] = sys_atmtyp_indices[jj];

            for (int idx=0; idx<3; idx++)
            {
                force_ptr_2b[0][idx] = &force[i             ][idx];
                force_ptr_2b[1][idx] = &force[sys_parent[jj]][idx];    
            }

            compute_2B(dist, dr, typ_idxs_2b, force_ptr_2b, stensor, energy);
        }
    }
    
    ////////////////////////
    // interate over 3b's 
    ////////////////////////
    
    if (poly_orders[1] > 0 )
    {
        for(int i=0; i<neighlist_3b.size(); i++)
        {
            ii = neighlist_3b[i][0];
            jj = neighlist_3b[i][1];
            kk = neighlist_3b[i][2];
        
            dist_3b[0] = get_dist(ii,jj,dr_3b[0]); 
            dist_3b[1] = get_dist(ii,kk,dr_3b[1]); 
            dist_3b[2] = get_dist(jj,kk,dr_3b[2]); 
        
            typ_idxs_3b[0] = sys_atmtyp_indices[ii];
            typ_idxs_3b[1] = sys_atmtyp_indices[jj];
            typ_idxs_3b[2] = sys_atmtyp_indices[kk];
        
            for (int idx=0; idx<3; idx++)
            {
                force_ptr_3b[0][idx] = &force[sys_parent[ii]][idx];
                force_ptr_3b[1][idx] = &force[sys_parent[jj]][idx];    
                force_ptr_3b[2][idx] = &force[sys_parent[kk]][idx];
            }    
        
            compute_3B(dist_3b, dr_3b, typ_idxs_3b, force_ptr_3b, stensor, energy);

        }
    }
    
    ////////////////////////
    // interate over 4b's 
    ////////////////////////

    if (poly_orders[2] > 0 )
    {
        for(int i=0; i<neighlist_4b.size(); i++)
        {
            ii = neighlist_4b[i][0];
            jj = neighlist_4b[i][1];
            kk = neighlist_4b[i][2];
            ll = neighlist_4b[i][3];
        
            dist_4b[0] = get_dist(ii,jj,dr_4b[0]); 
            dist_4b[1] = get_dist(ii,kk,dr_4b[1]); 
            dist_4b[2] = get_dist(ii,ll,dr_4b[2]); 
            dist_4b[3] = get_dist(jj,kk,dr_4b[3]); 
            dist_4b[4] = get_dist(jj,ll,dr_4b[4]); 
            dist_4b[5] = get_dist(kk,ll,dr_4b[5]);         

            typ_idxs_4b[0] = sys_atmtyp_indices[ii];
            typ_idxs_4b[1] = sys_atmtyp_indices[jj];
            typ_idxs_4b[2] = sys_atmtyp_indices[kk];
            typ_idxs_4b[3] = sys_atmtyp_indices[ll];        
        
            for (int idx=0; idx<3; idx++)
            {
                force_ptr_4b[0][idx] = &force[sys_parent[ii]][idx];
                force_ptr_4b[1][idx] = &force[sys_parent[jj]][idx];    
                force_ptr_4b[2][idx] = &force[sys_parent[kk]][idx];
                force_ptr_4b[3][idx] = &force[sys_parent[ll]][idx];
            }    
        
            compute_4B(dist_4b, dr_4b, typ_idxs_4b, force_ptr_4b, stensor, energy);
        }    
    }
    
    ////////////////////////
    // Finish pressure calculation
    ////////////////////////
    
    for (int idx=0; idx<9; idx++)
        *stensor[idx] /= vol;    
    
}
void serial_chimes_interface::build_layered_system( vector<string> & atmtyps)
{
    // Verify that an appropriate number of layers have been requested
    
    max_2b_cut = max_cutoff_2B(true); // true: do not write any info to stdout
    max_3b_cut = max_cutoff_3B(true);
    max_4b_cut = max_cutoff_4B(true);    
    
    double eff_lx = mag_a({hmat[0], hmat[1], hmat[2]}) * (2*n_layers + 1);
    double eff_ly = mag_a({hmat[3], hmat[4], hmat[5]}) * (2*n_layers + 1);
    double eff_lz = mag_a({hmat[6], hmat[7], hmat[8]}) * (2*n_layers + 1);
    
    if ((max_2b_cut>0.5*eff_lx)||(max_2b_cut>0.5*eff_ly)||(max_2b_cut>0.5*eff_lz))
    {
        cout << "ERROR: Maximum 2b cutoff is greater than half at least one box length." << endl;
        cout << "       Increase requested n_layers." << endl;
        cout << "       Max 2b cutoff:            " << max_2b_cut << endl;
        cout << "       Effective cell length(x): " << eff_lx << endl;
        cout << "       Effective cell length(y): " << eff_ly << endl;
        cout << "       Effective cell length(z): " << eff_lz << endl;
        cout << "       nlayers:                  " << n_layers << endl;
        exit(0);
    }        
    if (poly_orders[1] >0)
    {
        if ((max_3b_cut>0.5*eff_lx)||(max_3b_cut>0.5*eff_ly)||(max_3b_cut>0.5*eff_lz))
        {
            cout << "ERROR: Maximum 3b cutoff is greater than half at least one box length." << endl;
            cout << "       Increase requested n_layers." << endl;
            cout << "       Max 3b cutoff:            " << max_3b_cut << endl;
            cout << "       Effective cell length(x): " << eff_lx << endl;
            cout << "       Effective cell length(y): " << eff_ly << endl;
            cout << "       Effective cell length(z): " << eff_lz << endl;
            cout << "       nlayers:                  " << n_layers << endl;
            exit(0);
        }
    }
    if (poly_orders[2] >0)
    {
        if ((max_4b_cut>0.5*eff_lx)||(max_4b_cut>0.5*eff_ly)||(max_4b_cut>0.5*eff_lz))
        {
            cout << "ERROR: Maximum 4b cutoff is greater than half at least one box length." << endl;
            cout << "       Increase requested n_layers." << endl;
            cout << "       Max 4b cutoff:            " << max_4b_cut << endl;
            cout << "       Effective cell length(x): " << eff_lx << endl;
            cout << "       Effective cell length(y): " << eff_ly << endl;
            cout << "       Effective cell length(z): " << eff_lz << endl;
            cout << "       nlayers:                  " << n_layers << endl;
            exit(0);
        }
    }

    // Build the layers

    double tmp_x, tmp_y, tmp_z;

    for (int i=-n_layers; i<=n_layers; i++) // x
    {
        for (int j=-n_layers; j<=n_layers; j++) // y
        {
            for (int k=-n_layers; k<=n_layers; k++) // z
            {                
                if ((i==0)&&(j==0)&&(k==0))
                    continue;
                    
                for (int a=0; a<n_atoms; a++)
                {
                    n_ghost++;    
                
                    sys_atmtyps.push_back(atmtyps[a]);    
                    
                    sys_x.push_back(0.0); // Holder    
                    sys_y.push_back(0.0);
                    sys_z.push_back(0.0);
                    
                    // Transform into inverse space 

                    tmp_x = invr_hmat[0]*sys_x[a] + invr_hmat[1]*sys_y[a] + invr_hmat[2]*sys_z[a];
                    tmp_y = invr_hmat[3]*sys_x[a] + invr_hmat[4]*sys_y[a] + invr_hmat[5]*sys_z[a];
                    tmp_z = invr_hmat[6]*sys_x[a] + invr_hmat[7]*sys_y[a] + invr_hmat[8]*sys_z[a];
                    
                    tmp_x += i;
                    tmp_y += j;    
                    tmp_z += k;

                    sys_x[n_ghost-1] = hmat[0]*tmp_x + hmat[1]*tmp_y + hmat[2]*tmp_z;
                    sys_y[n_ghost-1] = hmat[3]*tmp_x + hmat[4]*tmp_y + hmat[5]*tmp_z;
                    sys_z[n_ghost-1] = hmat[6]*tmp_x + hmat[7]*tmp_y + hmat[8]*tmp_z;    
                    
                    sys_parent.push_back(a);
                }
            }
        }
    }
}
void serial_chimes_interface::build_neigh_lists()
{
    // Make the 2b neighbor lists
    
    neighlist_2b.resize(n_ghost);

    // Determine search distances

    double search_dist = max_2b_cut;
    
    if (max_3b_cut > search_dist)
        search_dist = max_3b_cut;
    if (max_4b_cut > search_dist)
        search_dist = max_4b_cut;    
    
    // Prepare bins
    
    int nbins_x = ceil((2 * n_layers+1) * extent_x/search_dist) + 2;
    int nbins_y = ceil((2 * n_layers+1) * extent_y/search_dist) + 2;
    int nbins_z = ceil((2 * n_layers+1) * extent_z/search_dist) + 2;     
    
    int total_bins = nbins_x * nbins_y * nbins_z;
    
    vector<vector<int> > bin(total_bins);

    for (int i=0; i<total_bins; i++) 
        vector<int>().swap(bin[i]);
    
    int bin_x_idx, bin_y_idx, bin_z_idx, ibin;

    // Populate bins    

    for(int i=0; i<n_ghost; i++)
    {
        bin_x_idx = floor( (sys_x[i] + extent_x * n_layers) / search_dist ) + 1;
        bin_y_idx = floor( (sys_y[i] + extent_y * n_layers) / search_dist ) + 1;
        bin_z_idx = floor( (sys_z[i] + extent_z * n_layers) / search_dist ) + 1;
        
        if ( bin_x_idx < 0 || bin_y_idx < 0 || bin_z_idx < 0 ) 
        {
            cout << "ERROR: Negative bin computed" << endl; // Check .xyz box lengths  and atom coords
            exit(0);
        }
        
        // Calculate bin BIN_IDX of the atom.
        int ibin = bin_x_idx + bin_y_idx * nbins_x + bin_z_idx * nbins_x* nbins_y;
        
        if ( ibin >= total_bins ) 
        {
            cout << "Error: ibin out of range\n";
            cout << ibin << " " << total_bins << endl;
            exit(1);
        }

        // Push the atom into the bin 
        bin[ibin].push_back(i);
        
    }

    // Generate neighbor lists on basis of bins
    
    for(int ai=0; ai<n_ghost; ai++)
    {
        bin_x_idx = floor( (sys_x[ai] + extent_x * n_layers) / search_dist ) + 1;
        bin_y_idx = floor( (sys_y[ai] + extent_y * n_layers) / search_dist ) + 1;
        bin_z_idx = floor( (sys_z[ai] + extent_z * n_layers) / search_dist ) + 1;

        // Loop over relevant bins only, not all atoms.
        
        int ibin, aj, ajj, ajend;
        
        for (int i=bin_x_idx-1; i<= bin_x_idx+1; i++)    //BIN_IDX_a1.X 
        {
            for (int j=bin_y_idx-1; j<=bin_y_idx+1; j++ ) //BIN_IDX_a1.Y
            {
                for (int k=bin_z_idx-1; k<=bin_z_idx+1; k++ ) // BIN_IDX_a1.Z
                {
                    ibin = i + j * nbins_x + k * nbins_x * nbins_y;

                    ajend = bin[ibin].size();
                    
                    for (int aj=0; aj<ajend; aj++) 
                    {
                        ajj = bin[ibin][aj];

                        if ( ajj == ai ) 
                            continue;

                        if ( ai <= sys_parent[ajj]) 
                            if (get_dist(ai,ajj) < search_dist )
                                neighlist_2b[ai].push_back(ajj);        
                    }
                }
            }
        }
    }    

    if ((poly_orders[1] == 0)&&(poly_orders[2]==0))
        return;    
    
    // Make the 3- and 4-b neighbor lists

    bool valid_3mer;
    bool valid_4mer;
    
    vector<int> tmp_3mer(3);
    vector<int> tmp_4mer(4);
    
    int jj, kk, ll;
    
    double dist;
    double dist_ij, dist_ik, dist_il, dist_jk, dist_jl, dist_kl;
    
    for(int i=0; i<n_atoms; i++)
    {
        for(int j=0; j<neighlist_2b[i].size(); j++) // Neighbors of i
        {
            valid_3mer = true;
            valid_4mer = true;
            
            jj = neighlist_2b[i][j];
            
            dist_ij = get_dist(i,jj);

            if (dist_ij >= max_3b_cut)
                valid_3mer = false;
            if (dist_ij >= max_4b_cut)
                valid_4mer = false;
                
            if (!valid_3mer && !valid_4mer)
                continue;
            
            for(int k=0; k<neighlist_2b[i].size(); k++)
            {                                
                kk = neighlist_2b[i][k];
                
                if (jj == kk)
                    continue;
                if (sys_parent[jj] > sys_parent[kk])
                    continue;
                
                dist_ik = get_dist(i,kk); // Check i/k distance
            
                if (dist_ik >= max_3b_cut)
                    valid_3mer = false;
                if (dist_ik >= max_4b_cut)
                    valid_4mer = false;
                    
                dist_jk = get_dist(jj,kk); // Check j/k distance
        
                if (dist_jk >= max_3b_cut)
                    valid_3mer = false;
                if (dist_jk >= max_4b_cut)
                    valid_4mer = false;                                        
                
                if (!valid_3mer && !valid_4mer)
                {
                    if(dist_ij<max_3b_cut)
                        valid_3mer = true;
                    
                    if(dist_ij<max_4b_cut)
                        valid_4mer = true;
                
                    continue;
                }
                
                // If we're here then we have a valid 3-mer ... add it to the 3b neighbor list    
                
                tmp_3mer[0] = i;
                tmp_3mer[1] = jj;
                tmp_3mer[2] = kk;
                
                if (valid_3mer)
                    neighlist_3b.push_back(tmp_3mer);
                
                // Continue on to 4-body list
                
                if (poly_orders[2] == 0)
                    continue;
                
                if (!valid_4mer)
                {
                    if(dist_ij<max_4b_cut)
                        valid_4mer = true;
                    continue;
                }
                
                for(int l=0; l<neighlist_2b[i].size(); l++) 
                {                                
                    ll = neighlist_2b[i][l];
                
                    if (jj == ll)
                        continue;
                    if (kk == ll)
                        continue;
                    if (sys_parent[jj] > sys_parent[ll])
                        continue;                
                    if (sys_parent[kk] > sys_parent[ll])
                        continue;                

                    if (get_dist(i ,ll) >= max_4b_cut) // Check i/l distance
                        continue;                

                    if (get_dist(jj,ll) >= max_4b_cut) // Check j/l distance
                        continue;    

                    if (get_dist(kk,ll) >= max_4b_cut) // Check k/l distance
                        continue;        
                    
                    // If we're here then we have a valid 4-mer ... add it to the 4b neighbor list    
                
                    tmp_4mer[0] = i;
                    tmp_4mer[1] = jj;
                    tmp_4mer[2] = kk;
                    tmp_4mer[3] = ll;
                
                    if (valid_4mer)
                        neighlist_4b.push_back(tmp_4mer);        
                }                                                                        
            }
        }
    }
    /*
    cout << "2B neighbor list is of length:" << neighlist_2b.size() << endl;
    cout << "3B neighbor list is of length:" << neighlist_3b.size() << endl;
    cout << "4B neighbor list is of length:" << neighlist_4b.size() << endl;
    */

}
double serial_chimes_interface::get_dist(int i,int j, vector<double> & rij)
{
    rij[0] = sys_x[j] - sys_x[i];
    rij[1] = sys_y[j] - sys_y[i];
    rij[2] = sys_z[j] - sys_z[i];
    
    return sqrt(rij[0]*rij[0] + rij[1]*rij[1] + rij[2]*rij[2]);
}
double serial_chimes_interface::get_dist(int i,int j)
{
    vector<double> rij(3);
    
    return get_dist(i,j,rij);
}
void serial_chimes_interface::set_atomtyp_indices()
{
    sys_atmtyp_indices.resize(n_ghost);

    for(int i=0; i<n_ghost; i++)
    {
        sys_atmtyp_indices[i] = -1;
        
        for (int j=0; j<type_list.size(); j++)
        {
            if ( sys_atmtyps[i] == type_list[j])
            {
                sys_atmtyp_indices[i] = j;
                break;
            }
        }
        
        if (sys_atmtyp_indices[i] == -1)
        {
            cout << "ERROR: Couldn't assign an atom type index for (index/type) " << sys_parent[i] << " " << sys_atmtyps[i] << endl;
            exit(0);
        }
    }
}







