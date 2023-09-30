<?php 
    class Airtel_Model extends CI_Model {

        public function __construct()
        {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert_or_update($data) {

            foreach($data as $element) {
                $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
                $exist= $this->db->get("airtel");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("airtel", $data);
                } else {
                $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
                return $this->db->update("airtel", $data);
            }
                
            }

            
        }

        public function get_normal_ci() {
            $sql= "SELECT a.*, b.*
                        FROM airtel_normale_ci a
                        LEFT JOIN igor_airtel_normale_ci b ON b.ref_igor = a.external_id
                        UNION 
                    SELECT a.*, b.*
                    FROM airtel_normale_ci a
                    RIGHT JOIN igor_airtel_normale_ci b ON b.ref_igor = a.external_id";
            
            $query= $this->db->query($sql);

            return $query->result();
        }


        public function get_normal_co() {
            $sql= "SELECT a.*, b.*
                        FROM airtel_normale_co a
                        LEFT JOIN igor_airtel_normale_co b ON b.ref_igor = a.external_id
                        UNION 
                    SELECT a.*, b.*
                    FROM airtel_normale_ci a
                    RIGHT JOIN igor_airtel_normale_co b ON b.ref_igor = a.external_id";
            
            $query= $this->db->query($sql);

            return $query->result();
        }
        public function insert_anormale_CO() {

            $query1 = "CREATE TEMPORARY TABLE temp_table_1 AS
                SELECT COMPTE, DATE_OPER, DATE_VAL, DEVISE, MONTANT, LIBELLE, OPER, EXPL, REF_IGOR, 
                NULL AS TRANSFER_ID, NULL AS transfer_date, NULL AS external_id, NULL AS account_no, NULL AS sender_msisdn, NULL AS dest_msisdn, NULL AS amount, NULL AS description, NULL AS service_name, NULL AS reference_number 
                FROM igor_airtel 
                WHERE OPER='CASHO' AND EXPL='AU' AND TRIM(REF_IGOR) NOT IN (SELECT TRIM(external_id) FROM airtel WHERE external_id IS NOT NULL AND (service_name='ChannelWallet To Bank Transfer' OR service_name='Wallet To Bank Transfer' OR service_name ='Auto Sweep Money Transfer') AND description='Transaction Success' AND reference_number NOT LIKE 'RO%')";
            
            $this->db->query($query1);


            $query2 = "CREATE TEMPORARY TABLE temp_table_2 AS
                SELECT NULL AS COMPTE, NULL AS DATE_OPER, NULL AS DATE_VAL, NULL AS DEVISE, NULL AS MONTANT, NULL AS LIBELLE, NULL AS OPER, NULL AS EXPL, NULL AS REF_IGOR, TRANSFER_ID, transfer_date, external_id, account_no, sender_msisdn, dest_msisdn, amount, description, service_name, reference_number 
                FROM airtel 
                WHERE description='Transaction Success' AND (service_name='Wallet To Bank Transfer' OR service_name='ChannelWallet To Bank Transfer' OR service_name ='Auto Sweep Money Transfer') AND reference_number NOT LIKE 'RO%' AND TRIM(external_id) NOT IN (SELECT TRIM(REF_IGOR) FROM igor_airtel WHERE REF_IGOR IS NOT NULL AND OPER='CASHO' AND EXPL='AU')";
            
            $this->db->query($query2);
            
            
            $query3 = "INSERT INTO airtel_anomalie_co (compte, date_oper, date_val, devise, montant, libelle, oper, expl, ref_igor, transfer_id, tranfer_date, external_id, account_no, sender_msisdn, dest_msisdn, amount, description, service_name, reference_number, etat, date_regul)
                SELECT COMPTE, DATE_OPER, DATE_VAL, DEVISE, MONTANT, LIBELLE, OPER, EXPL, REF_IGOR, TRANSFER_ID, transfer_date, external_id, account_no, sender_msisdn, dest_msisdn, amount, description, service_name, reference_number, 'Non' AS etat, NULL AS date_regul
                FROM temp_table_1 AS t1
                WHERE NOT EXISTS (
                    SELECT 1
                    FROM airtel_anomalie_co AS aac
                    WHERE (aac.ref_igor = t1.REF_IGOR OR aac.transfer_id = t1.TRANSFER_ID)
                )";
            
            
            $this->db->query($query3);

            $query4 = "INSERT INTO airtel_anomalie_co (compte, date_oper, date_val, devise, montant, libelle, oper, expl, ref_igor, transfer_id, tranfer_date, external_id, account_no, sender_msisdn, dest_msisdn, amount, description, service_name, reference_number, etat, date_regul)
                SELECT COMPTE, DATE_OPER, DATE_VAL, DEVISE, MONTANT, LIBELLE, OPER, EXPL, REF_IGOR, TRANSFER_ID, transfer_date, external_id, account_no, sender_msisdn, dest_msisdn, amount, description, service_name, reference_number, 'Non' AS etat, NULL AS date_regul
                FROM temp_table_2 AS t2
                WHERE NOT EXISTS (
                    SELECT 1
                    FROM airtel_anomalie_co AS aac
                    WHERE (aac.ref_igor = t2.REF_IGOR OR aac.transfer_id = t2.TRANSFER_ID)
                )";
            
            
            $this->db->query($query4);

            $this->db->query("DROP TEMPORARY TABLE temp_table_1");
            $this->db->query("DROP TEMPORARY TABLE temp_table_2");
        }
        public function get_anormale_CO() {
            $this->insert_anormale_CO();

            $this->db->select("*");
            $this->db->from("airtel_anomalie_co");
            $query= $this->db->get();

            return $query->result();
        }

        public function get_anormale_CI() {

            $this->insererAnomalieCI();
            $this->db->select("*");
            $this->db->from("airtel_anomalie_ci");
            $query= $this->db->get();

            return $query->result();

        }

        public function insererAnomalieCI() {
            $query1= "CREATE TEMPORARY TABLE temp_1 AS 
                SELECT COMPTE, DATE_OPER, DATE_VAL, DEVISE, MONTANT, LIBELLE, OPER, EXPL, REF_IGOR, NULL AS TRANSFER_ID, NULL AS transfer_date, NULL AS external_id,
                                    NULL AS account_no, NULL AS sender_msisdn, NULL AS dest_msisdn, NULL AS amount, NULL AS description, NULL AS service_name, NULL AS reference_number
                                        FROM igor_airtel
                                        WHERE OPER='CASHI' AND EXPL='AU' AND TRIM(REF_IGOR) NOT IN (SELECT TRIM(external_id) FROM airtel WHERE external_id IS NOT NULL AND (service_name='Bank to Wallet Transfer' OR service_name='Channel Bank to Wallet Transfer') AND description='Transaction Success' AND reference_number NOT LIKE 'RO%')";
            $this->db->query($query1);
            
            $query2 ="CREATE TEMPORARY TABLE temp_2 AS 

                SELECT NULL AS COMPTE, NULL AS DATE_OPER, NULL AS DATE_VAL, NULL AS DEVISE, NULL AS MONTANT, NULL AS LIBELLE, NULL AS OPER, NULL AS EXPL, NULL AS REF_IGOR, TRANSFER_ID, transfer_date, external_id, account_no, sender_msisdn, dest_msisdn, amount, description, service_name,
                                reference_number 
                                FROM airtel
                                WHERE description='Transaction Success' AND (service_name='Bank to Wallet Transfer' OR service_name='Channel Bank to Wallet Transfer') AND  reference_number NOT LIKE 'RO%' AND TRIM(external_id) NOT IN (SELECT TRIM(REF_IGOR) FROM igor_airtel WHERE REF_IGOR IS NOT NULL AND OPER='CASHI' AND EXPL='AU')";

            $this->db->query($query2);
            
            $query3= "INSERT INTO airtel_anomalie_ci (compte, date_oper, date_val, devise, montant, libelle, oper, expl, ref_igor, transfer_id, transfer_date, external_id, account_no, sender_msisdn, dest_msisdn, amount, description, service_name, reference_number, etat, date_regul)
            SELECT COMPTE, DATE_OPER, DATE_VAL, DEVISE, MONTANT, LIBELLE, OPER, EXPL, REF_IGOR, TRANSFER_ID, transfer_date, external_id, account_no, sender_msisdn, dest_msisdn, amount, description, service_name, reference_number, 'Non' AS etat, NULL AS date_regul FROM temp_1 AS t1 
            
            WHERE NOT EXISTS (
                SELECT 1 FROM airtel_anomalie_ci AS aaci WHERE (aaci.ref_igor = t1.REF_IGOR OR aaci.transfer_id= t1.TRANSFER_ID))";

            $this->db->query($query3);

            $query4 = "INSERT INTO airtel_anomalie_ci (compte, date_oper, date_val, devise, montant, libelle, oper, expl, ref_igor, transfer_id, transfer_date, external_id, account_no, sender_msisdn, dest_msisdn, amount, description, service_name, reference_number, etat, date_regul) SELECT COMPTE, DATE_OPER, DATE_VAL, DEVISE, MONTANT, LIBELLE, OPER, EXPL, REF_IGOR, TRANSFER_ID, transfer_date, external_id, account_no, sender_msisdn, dest_msisdn, amount, description, service_name, reference_number, 'Non' AS etat, NULL AS date_regul FROM temp_2 AS t2 WHERE NOT EXISTS (SELECT 1 FROM airtel_anomalie_ci AS aaci WHERE (aaci.ref_igor = t2.REF_IGOR OR aaci.transfer_id= t2.TRANSFER_ID))";

            $this->db->query($query4);

            $this->db->query("DROP TEMPORARY TABLE temp_1");
            $this->db->query(" DROP TEMPORARY TABLE temp_2");


           
            
    
        }

        public function update_anormale_CO($id, $etat, $date_regul) {
            
            $this->db->set('etat', $etat);
            $this->db->set('date_regul', $date_regul);
            $this->db->where('id', $id);
            $this->db->update('airtel_anomalie_co');
        }

        public function update_anormale_CI($id, $etat, $date_regul) {
            $this->db->set('etat', $etat);
            $this->db->set('date_regul', $date_regul);
            $this->db->where('id', $id);
            $this->db->update('airtel_anomalie_ci');
        }

        
    }
?>