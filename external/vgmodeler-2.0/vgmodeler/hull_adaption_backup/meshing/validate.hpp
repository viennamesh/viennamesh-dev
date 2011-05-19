#ifndef VALIDATE_HPP
#define VALIDATE_HPP



void GetPureBadness(Mesh & mesh, ARRAY<double> & pure_badness,
		    const BitArray & isnewpoint);
double Validate(const Mesh & mesh, ARRAY<ElementIndex> & bad_elements,
		const ARRAY<double> & pure_badness, double max_worsening);
void RepairBisection(Mesh & mesh, ARRAY<ElementIndex> & bad_elements, const BitArray & isnewpoint, Refinement & refinement,
		     const ARRAY<double> & pure_badness, double max_worsening,
		     const ARRAY< ARRAY<int,PointIndex::BASE>* > & idmaps);

#endif // VALIDATE_HPP
