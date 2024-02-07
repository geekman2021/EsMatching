<div class=" container-fluid mt-5">
    <ul class="nav nav-tabs">
        <li class="nav-item col-sm-3">
            <a class="nav-link" data-toggle="tab" href="#tabDat">Debit a Tort</a>
        </li>
        <li class="nav-item col-sm-3">
            <a class="nav-link" data-toggle="tab" href="#tabCat">Credit a Tort</a>
        </li>
        <li class="nav-item col-sm-2">
            <a class="nav-link" data-toggle="tab" href="#tabMvts">MVTS</a>
        </li>
        <li class="nav-item col-sm-2">
            <a class="nav-link" data-toggle="tab" href="#tabAdmin">Admin</a>
        </li>
        <li class="nav-item col-sm-2">
            <a class="nav-link" data-toggle="tab" href="#tabVi">VI</a>
        </li>
    </ul>
</div>


<?php $this->load->view("pages/regul/telma/tabDat"); ?>
<?php $this->load->view("pages/regul/telma/tabCat"); ?>
<?php $this->load->view("pages/regul/telma/tabMvts"); ?>
<?php $this->load->view("pages/regul/telma/tabAdmin"); ?>
<?php $this->load->view("pages/regul/telma/tabVi"); ?>





<script>
    $(document).ready(function(e) {
        $("#tableDat").DataTable({
            scrollX: true,
            language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
            }
        });
        $("#tableCat").DataTable({
            scrollX: true,
            language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
            }
        });
        $("#tableAdmin").DataTable({
            language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
            }
        });
        $("#tableMvts").DataTable({
            language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
            }
        });
        $("#tableVi").DataTable({
            language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
            }
        });
    });
</script>






