<?php
    class Igor_Airtel_Anomalie_Model extends CI_Model {

        public function __construct() {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert_or_update_ci($data) {

            $this->db->where("REF_IGOR", $data["REF_IGOR"]);
            $exist= $this->db->get("igor_airtel_anomalie_ci");

            if($exist->num_rows() == 0) {
                return $this->db->insert("igor_airtel_anomalie_ci", $data);
            } else {
                $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                return $this->db->update("igor_airtel_anomalie_ci", $data);
            }
        }

        public function insert_or_update_vi($data) {
            $this->db->where("REF_IGOR", $data["REF_IGOR"]);
            $exist= $this->db->get("igor_airtel_anomalie_vi");

            if($exist->num_rows() == 0) {
                return $this->db->insert("igor_airtel_anomalie_vi", $data);
            } else {
                $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                return $this->db->update("igor_airtel_anomalie_vi", $data);
            }
        }

        public function insert_or_update_co($data) {
            
            $this->db->where("REF_IGOR", $data["REF_IGOR"]);
            $exist= $this->db->get("igor_airtel_anomalie_co");

            if($exist->num_rows() == 0) {
                return $this->db->insert("igor_airtel_anomalie_co", $data);
            } else {
                $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                return $this->db->update("igor_airtel_anomalie_co", $data);
            }
        }

        public function get_anomalie_ci() {
            $this->db->select('igor_airtel_anomalie_ci.*');
            $this->db->from('igor_airtel_anomalie_ci');
            $this->db->join('boa_airtel_nonau', 'igor_airtel_anomalie_ci.REF_IGOR = boa_airtel_nonau.ref_igor', 'left');
            $this->db->where('boa_airtel_nonau.ref_igor IS NULL');
            $query = $this->db->get();
            return $query->result();
        }

        public function get_anomalie_co() {
            $this->db->select('*');
            $this->db->from('igor_airtel_anomalie_co');
            $this->db->where_not_in('REF_IGOR', "SELECT REF_IGOR FROM airtel_anomalie_co", false);
            $query = $this->db->get();
            return $query->result();
        }


        

        public function delete_ci($igor) {
            $this->db->where("REF_IGOR", $igor);
            $this->db->delete("igor_airtel_anomalie_co");
        }

        public function get_vi() {
            $sql= "SELECT DISTINCT igor_airtel_anomalie_vi.*
                        FROM igor_airtel_anomalie_vi
                    LEFT JOIN airtel_deallocation ON airtel_deallocation.reference_number = igor_airtel_anomalie_vi.reference_number WHERE airtel_deallocation.reference_number IS NULL ";
            $query= $this->db->query($sql);

            return $query->result();
            
        }

        public function delete_vi($ref_igor) {
            $this->db->where("REF_IGOR", $ref_igor);
            $this->db->delete("igor_airtel_anomalie_vi");
        }

        public function update_anomalie_co($ref_igor, $etat, $date_regul) {
            $this->db->set('etat', $etat);
            $this->db->set('date_regul', $date_regul);
            $this->db->where('REF_IGOR', $ref_igor);
            $this->db->update('igor_airtel_anomalie_co');
        }

        public function update_anomalie_ci($ref_igor, $etat, $date_regul) {
            $this->db->set('etat', $etat);
            $this->db->set('date_regul', $date_regul);
            $this->db->where('REF_IGOR', $ref_igor);
            $this->db->update('igor_airtel_anomalie_ci');
        }

        public function get_ci() {
            $this->db->select("*");
            $this->db->from("igor_airtel_anomalie_ci");
            return $this->db->get();
        }

        public function get_co() {
            $this->db->select("*");
            $this->db->from("igor_airtel_anomalie_co");
        }
        
    }

?>