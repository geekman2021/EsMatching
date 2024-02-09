<div class="tab-content">
    <div id="tabDat" class="container-fluid mt-1 tab-pane "><br>  
        <div class="row">
            <div class="col-sm-6 pl-4">
                <label for="dateDebut">Date Debut</label>
                <input type="text" class="form-control" id="min" name="min">
            </div>
            <div class="col-sm-6 pr-4">
                <label for="dateDebut">Date Fin</label>
                <input type="text" class="form-control" id="max" name="max">
            </div> 
        </div> 
        <div class="container-fluid mt-5">
            <div class="card">
                <div class="card-header">
                    <b>Liste Debit a Tort Regularisé</b>
                </div>
                <div class="card-body">
                    <table class="table table-bordered" id="tableDat">
                <thead style="text-align: center ;">
                    <tr>
                        <th>Date_Oper</th>
                        <th>date_val</th>
                        <th>montant</th>
                        <th>Devise</th>
                        <th>Oper</th>
                        <th>Expl</th>
                        <th>ReferenceIgor</th>
                        <th>Numero</th>
                        <th>Action</th>
                    </tr>
                </thead>
                <tbody>
                </tbody>
            </table>
                </div>
            </div>
            
        </div>
    </div>
    <?php $this->load->view("pages/regul/orange/modal/modalDat") ?>