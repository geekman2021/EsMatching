<?php 
    class Airtel_Anomalie_Model extends CI_Model {

        public function __construct()
        {
            parent::__construct();
            $this->load->database("db1");
        }

        public function insert_or_update_ci($data) {
            $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
                $exist= $this->db->get("airtel_anomalie_ci");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("airtel_anomalie_ci", $data);
                } else {
                $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
                
                return $this->db->update("airtel_anomalie_ci", $data);
            }
        }

        
        public function insert_or_update_co($data) {

            $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
            $exist= $this->db->get("airtel_anomalie_co");
            if($exist->num_rows() == 0) {
                return $this->db->insert("airtel_anomalie_co", $data);
            } else {
            $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
            
            return $this->db->update("airtel_anomalie_co", $data);
            }
        }

        public function insert_or_update_rb($data) {

            $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
            $exist= $this->db->get("airtel_rollback");
            if($exist->num_rows() == 0) {
                return $this->db->insert("airtel_rollback", $data);
            } else {
            $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
            
            return $this->db->update("airtel_rollback", $data);
            }
        }

        public function insert_or_update_deallocation($data) {

            $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
            $exist= $this->db->get("airtel_deallocation");
            if($exist->num_rows() == 0) {
                return $this->db->insert("airtel_deallocation", $data);
            } else {
            $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
            
            return $this->db->update("airtel_deallocation", $data);
            }
        }

        public function insert_or_update_ambiguous($data) {

            $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
            $exist= $this->db->get("airtel_ambiguous");
            if($exist->num_rows() == 0) {
                return $this->db->insert("airtel_ambiguous", $data);
            } else {
            $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
            
            return $this->db->update("airtel_ambiguous", $data);
            }
        }

        public function get_anomalie_ci() {
            $this->db->select("*");
            $this->db->from("airtel_anomalie_ci");
            $query= $this->db->get();

            return $query->result();
        }

        public function get_anomalie_co() {
            $this->db->select("*");
            $this->db->from("airtel_anomalie_co");
            $query= $this->db->get();

            return $query->result();
        }

        public function get_deallocation() {
            $this->db->select('*');
            // $this->db->select_sum('amount', 'total');
            $this->db->from('airtel_deallocation');
            // $this->db->group_by('reference_number');
            $query = $this->db->get();
            return $query->result();
        }

        public function get_ambiguous() {
            $this->db->select("*");
            $this->db->from("airtel_ambiguous");
            $query= $this->db->get();
            return $query->result();
        }

        public function delete_ambiguous($transfer_id) {
            $this->db->where("TRANSFER_ID", $transfer_id);
            $this->db->delete("utilisateurs");
        }

        public function update_anomalie_co($id, $etat, $date_regul) {
            $this->db->set('etat', $etat);
            $this->db->set('date_regul', $date_regul);
            $this->db->where('id', $id);
            $this->db->update('airtel_anomalie_co');
        }

        public function update_anomalie_ci( $id, $etat, $date_regul) {
            $this->db->set('etat', $etat);
            $this->db->set('date_regul', $date_regul);
            $this->db->where('id', $id);
            $this->db->update('airtel_anomalie_ci');
        }

        public function update_anomalie_deallo( $id, $etat, $date_regul) {
            $this->db->set('etat', $etat);
            $this->db->set('date_regul', $date_regul);
            $this->db->where('id', $id);
            $this->db->update('airtel_deallocation');
        }

        public function delete_deallocation ($reference_number) {
            $this->db->where("reference_number", $reference_number);
            $this->db->delete("airtel_deallocation");
        }

    }
?>