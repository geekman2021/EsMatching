<?php 
    class Ecart_Airtel_Model extends CI_Model {

        public function __construct()
        {
            parent::__construct();
            $this->load->database("default");
        }

        public function get_ecart_airtel() {
            $sql = "SELECT TRANSFER_ID, transfer_date, external_id, account_no, sender_msisdn, dest_msisdn,amount, solde, description, service_name, reference_number FROM airtel_ambiguous WHERE airtel_ambiguous.TRANSFER_ID NOT IN (SELECT rollback_reference_number FROM airtel_rollback)
                                                        UNION
                    SELECT TRANSFER_ID, transfer_date, external_id, account_no, sender_msisdn, dest_msisdn, amount, solde, description, service_name, reference_number FROM airtel_deallocation WHERE REF_IGOR IS NULL 
            
            ";
    
            $query = $this->db->query($sql);
            return $query->result();
        }

        public function get_ecart_boa() {
            $sql= "SELECT igor_airtel_anomalie_ci.*
                        FROM igor_airtel_anomalie_ci
                    LEFT JOIN boa_airtel_nonau ON igor_airtel_anomalie_ci.REF_IGOR = boa_airtel_nonau.ref_igor
                        WHERE boa_airtel_nonau.ref_igor IS NULL

                    
                    UNION 

                    SELECT *
                    FROM igor_airtel_anomalie_co
                    WHERE REF_IGOR NOT IN (SELECT REF_IGOR FROM airtel_anomalie_co)

                    UNION 

                    SELECT *
                    FROM igor_airtel_anomalie_vi
                    WHERE etat IS NULL
                    GROUP BY REF_IGOR;
                ";
            

            
            $query= $this->db->query($sql);


            return $query->result();
        }
    }
?>